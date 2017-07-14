#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
//#include <sys/stat.h>
#include <semaphore.h>
#include "lib_sem.h"
#include <string.h>
#include <time.h>

// Structure Start

typedef struct sem_type
{
	pthread_mutex_t mutex_lock;
	pthread_cond_t cond_var;
	int count;
} m_sem_t;

typedef struct barrier_t
{
	sem_t sem;
	pthread_mutex_t lock;
	pthread_mutex_t lock2;
	pthread_cond_t cond;
	pthread_cond_t cond2;
	char flag;
	int count;
} barrier_t;
// Structure End

// Function definition start
barrier_t *barrier_var_create( char *arg_barrier_var_create, int arg_philosopher_count_create )
{
	int fd_create, val;
	barrier_t *barrier_var_create;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	if( pthread_mutexattr_init( &mutex_attr ) < 0 )
	{
		printf("Error initializing mutex attribute for barrier\n");
		exit(1);
	}
	if( pthread_condattr_init( &cond_attr ) < 0 )
	{
		printf("Error initializing cond attribute for barrier\n");
		exit(1);
	}
	if( pthread_mutexattr_setpshared( &mutex_attr, PTHREAD_PROCESS_SHARED ) < 0 )
	{
		printf("Error setting mutex attribute to process shared for barrier\n");
		exit(1);
	}
	if( pthread_condattr_setpshared( &cond_attr, PTHREAD_PROCESS_SHARED ) < 0 )
	{
		printf("Error setting cond attribute to process shared for barrier\n");
		exit(1);
	}
	fd_create = open( arg_barrier_var_create, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR );
	if( fd_create == -1 )
	{
		fd_create = open( arg_barrier_var_create, O_RDWR | O_TRUNC );
		if( fd_create == -1 )
		{
			printf("Error creating backing file %s\n", arg_barrier_var_create);
			exit(1);
		}
	}
	if( ftruncate( fd_create, sizeof( *barrier_var_create ) ) == -1 )
	{	
		printf("Error truncating backing file %s\n", arg_barrier_var_create);
		exit(1);
	}
	barrier_var_create = ( barrier_t * )mmap( NULL, sizeof( *barrier_var_create ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_create, 0 );
	if( barrier_var_create == MAP_FAILED )
	{
		printf("Error accessing backing file %s\n", arg_barrier_var_create);
		exit(1);
	}
	//printf("count while creating is %d\n", arg_philosopher_count_create);
	sem_init( &barrier_var_create->sem, 0, arg_philosopher_count_create );
	sem_getvalue( &barrier_var_create->sem, &val );
	//printf("The value after init is %d\n", val );
	pthread_mutex_init( &barrier_var_create->lock, &mutex_attr );
	pthread_mutex_init( &barrier_var_create->lock2, &mutex_attr );
	pthread_cond_init( &barrier_var_create->cond, &cond_attr );
	pthread_cond_init( &barrier_var_create->cond2, &cond_attr );
	barrier_var_create->flag = 'F';
	barrier_var_create->count = arg_philosopher_count_create;
	close( fd_create );	
	return barrier_var_create;
}

barrier_t *barrier_var_open( char *arg_barrier_var_open )
{
	int fd_open;
	barrier_t *barrier_var_open;
	//printf("Entered barrier open\n");
	fd_open = open( arg_barrier_var_open, O_RDWR );
	if( fd_open == -1)
	{
		printf("Error opening backing file %s\n", arg_barrier_var_open);
		exit(1);
	}
	barrier_var_open = ( barrier_t * )mmap( NULL, sizeof( *barrier_var_open ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_open, 0 );
	if( barrier_var_open == MAP_FAILED )
	{
		printf("Error accessing backing file %s\n", arg_barrier_var_open);
		exit(1);
	}
	close( fd_open );
	//printf("Exiting barrier open\n");
	return barrier_var_open;
}

void barrier_wait( barrier_t *arg_barrier_var_wait, int arg_philosopher_count, int id )
{
	char print_char[100];
	memset( print_char, '\0', 100 );
	int val;
	struct timespec tns;
	//sprintf( print_char, "I'm philosopher %d and I just entered barrier\n", id );
	//write( 1, print_char, sizeof( print_char ) );
	sem_wait( &arg_barrier_var_wait->sem );
		//printf("philosopher %d entered sem wait with count %d\n", id, arg_barrier_var_wait->count); 
		pthread_mutex_lock( &arg_barrier_var_wait->lock );
			arg_barrier_var_wait->count--;
			while( arg_barrier_var_wait->count != 0 )
			{	
				pthread_cond_wait( &arg_barrier_var_wait->cond, &arg_barrier_var_wait->lock );
			}
			pthread_cond_broadcast( &arg_barrier_var_wait->cond );
		pthread_mutex_unlock( &arg_barrier_var_wait->lock );
	sem_post( &arg_barrier_var_wait->sem );
	sprintf( print_char, "Barrier done philosopher %d\n", id);
	write( 1, print_char, sizeof( print_char ) );
	pthread_mutex_lock( &arg_barrier_var_wait->lock2 );
		arg_barrier_var_wait->flag = 'F';
		sem_getvalue( &arg_barrier_var_wait->sem, &val );
		while( val != arg_philosopher_count )
		{
			pthread_cond_wait( &arg_barrier_var_wait->cond2, &arg_barrier_var_wait->lock2 );
			sem_getvalue( &arg_barrier_var_wait->sem, &val );
		}
		arg_barrier_var_wait->count++;
		pthread_cond_broadcast( &arg_barrier_var_wait->cond2 );
		if( arg_barrier_var_wait->count ==  arg_philosopher_count )
			arg_barrier_var_wait->flag = 'T';
		//sprintf( print_char, "I'm atomic philosopher %d and my count is %d and my flag is %c\n", id, arg_barrier_var_wait->count, arg_barrier_var_wait->flag);
		//write( 1, print_char, sizeof( print_char ) );
	pthread_mutex_unlock( &arg_barrier_var_wait->lock2 );
	while( arg_barrier_var_wait->flag != 'T' );
	
}

void barrier_var_close( barrier_t *arg_barrier_var_close )
{
	munmap( arg_barrier_var_close, sizeof( *arg_barrier_var_close ) );
}
	
char *neighbour_status_create( char *arg_neighbour_status_create, int arg_philosopher_count_create )
{
	int index_create;
	int fd_create;
	char *neighbour_status_create;
	//printf("Entered neighbour status create\n");
	fd_create = open( arg_neighbour_status_create, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
	if( fd_create == -1 )
	{
		fd_create = open( arg_neighbour_status_create, O_RDWR | O_TRUNC );
		if( fd_create == -1 )
		{
				printf("Error creating backing file %s\n", arg_neighbour_status_create);
				exit(1);
		}
	}
	if( ftruncate( fd_create, arg_philosopher_count_create * sizeof( char ) ) == -1 )
	{
		printf("Error trucating file %s\n", arg_neighbour_status_create);
		exit(1);
	}
	neighbour_status_create = ( char * )mmap( NULL, arg_philosopher_count_create * sizeof( char ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_create, 0 );
	if( neighbour_status_create == MAP_FAILED )
	{
		printf("Error accessing backing file %s\n", arg_neighbour_status_create);
		exit(1);
	}
	for( index_create = 0; index_create < arg_philosopher_count_create; index_create++ )
		neighbour_status_create[ index_create ] = 'T';
	close( fd_create );
	//printf("Exiting neighbour status create\n");
	return neighbour_status_create;
}

char *neighbour_status_open( char *arg_neighbour_status_open, int arg_philosopher_count_open )
{
	int fd_open;
	char *neighbour_status_open;
	//printf("Entered neighbour status open\n");
	fd_open = open( arg_neighbour_status_open, O_RDWR );
	if( fd_open == -1 )
	{
		printf("Error opening backing file %s\n", arg_neighbour_status_open);
		exit(1);
	}
	neighbour_status_open = ( char * )mmap( NULL, arg_philosopher_count_open * sizeof( char ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_open, 0 );
	if( neighbour_status_open == MAP_FAILED )
	{
		printf("Error mapping to backing file %s\n", arg_neighbour_status_open);
		exit(1);
	}
	close( fd_open );
	//printf("Exiting neighbour status open\n");
	return neighbour_status_open;
}

void neighbour_status_close( char *arg_neighbour_status_close, int arg_philosopher_count_close )
{
	//printf("Entered neighbour status close\n");
	munmap( arg_neighbour_status_close, arg_philosopher_count_close * sizeof( char ) );
	//printf("Exiting neighbour status close\n");
}

m_sem_t *m_sem_create( char *arg_sem_create, int arg_phil_count )
{
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	//printf("Entered sem create\n");
	if( pthread_mutexattr_init( &mutex_attr ) < 0 )
	{
		printf("Error initializing mutex attr\n");
		exit(1);
	}
	if( pthread_condattr_init( &cond_attr ) < 0 )
	{
		printf("Error initializing cond attr\n");
		exit(1);
	}
	if( pthread_mutexattr_setpshared( &mutex_attr, PTHREAD_PROCESS_SHARED ) < 0 )
	{
		printf("Errir setting mutes attr pshared\n");
		exit(1);
	}
	if( pthread_condattr_setpshared( &cond_attr, PTHREAD_PROCESS_SHARED ) < 0 )
	{
		printf("Error setting cond attr pshared\n");
		exit(1);
	}
	int fd_create = open( arg_sem_create, O_RDWR | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR );
	if( fd_create == -1 )
	{
		fd_create = open( arg_sem_create, O_RDWR | O_TRUNC );
		if( fd_create == -1 )
		{
			printf("Error creating backing file\n");
			exit(1);
		}
	}
	if( ftruncate( fd_create, sizeof( m_sem_t ) ) == -1 )
	{
		printf("Error truncating file\n");
		exit(1);
	}
	m_sem_t *sem_create = ( m_sem_t * )mmap( NULL, sizeof( *sem_create ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_create, 0 );
	close( fd_create );
	sem_create->count = arg_phil_count;
	pthread_mutex_init( &sem_create->mutex_lock, &mutex_attr );
	pthread_cond_init( &sem_create->cond_var, &cond_attr );
	//printf("Exiting sem create\n");
	return sem_create;
}

m_sem_t *m_sem_open( char *arg_sem_open )
{
	//printf("Entered sem open\n");
	int fd_open = open( arg_sem_open, O_RDWR );
	if( fd_open < 0 )
	{	
		printf("Error opening backing file\n");
		exit(1);
	}	
	m_sem_t *sem_open = ( m_sem_t * )mmap( NULL, sizeof( *sem_open ), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd_open, 0 );
	close( fd_open );
	//printf("Exiting sem open\n");
	return sem_open;
}

void m_sem_post( m_sem_t *arg_sem_post, char *arg_neighbour_status_post, int arg_philosopher_id_post )
{
	char print_char[100];
	memset( print_char, '\0', 100 );
	//printf("Entered sem post\n");
	pthread_mutex_lock( &arg_sem_post->mutex_lock );
		//if( arg_sem_post->count <= 0 )
			pthread_cond_broadcast( &arg_sem_post->cond_var );
		arg_sem_post->count += 2;
		arg_neighbour_status_post[ arg_philosopher_id_post ]  = 'T';
		sprintf(print_char, "I'm Philosopher %d and my current status is Thinking\n", arg_philosopher_id_post); //, arg_neighbour_status_post[ arg_philosopher_id_post ]);
		write(1, print_char, sizeof( print_char ) );
	//printf("Exiting sem post\n");
	pthread_mutex_unlock( &arg_sem_post->mutex_lock );
}

void m_sem_wait( m_sem_t *arg_sem_wait, char *arg_neighbour_status_wait, int arg_philosopher_id_wait )
{
	char print_char[100];
	memset( print_char, '\0', 100 );
	//printf("Entered sem wait\n");
	int left = ( ( arg_philosopher_id_wait - 1 ) < 0 ) ? (  strlen( arg_neighbour_status_wait )  - 1 ) : ( arg_philosopher_id_wait - 1 );
	int right = ( ( arg_philosopher_id_wait + 1 ) > ( strlen( arg_neighbour_status_wait ) - 1 ) ) ? 0 : ( arg_philosopher_id_wait + 1 );
	pthread_mutex_lock( &arg_sem_wait->mutex_lock );
		arg_neighbour_status_wait[ arg_philosopher_id_wait ] = 'H';
                sprintf( print_char, "I'm Philosopher %d and my current status is Hungry\n", arg_philosopher_id_wait); //, arg_neighbour_status_wait[ arg_philosopher_id_wait ]);
                write( 1, print_char, sizeof( print_char ) );
		while( arg_sem_wait->count <= 1 || arg_neighbour_status_wait[ left ] == 'E' || arg_neighbour_status_wait[ right ] == 'E' )
		{
			arg_neighbour_status_wait[ arg_philosopher_id_wait ] = 'H';
			//sprintf( print_char, "I'm Philosopher %d and my current status is %c\n", arg_philosopher_id_wait, arg_neighbour_status_wait[ arg_philosopher_id_wait ]);
			//write( 1, print_char, sizeof( print_char ) );
			pthread_cond_wait( &arg_sem_wait->cond_var, &arg_sem_wait->mutex_lock );
		}
		arg_sem_wait->count -= 2;
		arg_neighbour_status_wait[ arg_philosopher_id_wait ] = 'E';
		sprintf( print_char,"I'm Philosopher %d and my current status is Eating\n", arg_philosopher_id_wait); //, arg_neighbour_status_wait[ arg_philosopher_id_wait ]);
		write( 1, print_char, sizeof( print_char ) );
		pthread_cond_broadcast( &arg_sem_wait->cond_var );
	//printf("Exiting sem wait\n");
	pthread_mutex_unlock( &arg_sem_wait->mutex_lock );

}
void m_sem_close( m_sem_t *arg_sem_close )
{
	//printf("Entered sem close\n");
	munmap( arg_sem_close, sizeof( *arg_sem_close ) );
	//printf("exiting sem cloe\n");
}

// Function definition end
