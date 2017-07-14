#include "lib_sem.h"

void main(int argc, char **argv)
{
	int i;
	// Get commandline arguments
	int iterations = atoi( argv[1] );
	int philosopher_count = atoi( argv[2] );
	int philosopher_id = atoi( argv[3] );
	//
	// Open a pointer to the barrier var
	barrier_t *barrier_var = barrier_var_open( "./barrier_var_bak.dat" );
	//
		
	// Open a pointer to the neighbour status table
	char *neighbour_status = neighbour_status_open( "./neighbour_status_bak.dat", philosopher_count );
	//
	
	// open a pointer to the semaphore backing file
	m_sem_t *sem_var = m_sem_open( "./sem_var_bak.dat" );
	//
	
	// Make all the processes go through the barrier
	barrier_wait( barrier_var, philosopher_count, philosopher_id );
	//
	printf("I'm philosopher %d and my current status is Thinking\n", philosopher_id);
	// Start the loop and do the Philosopher stuff
	for( i = 0; i < iterations; i++ )
	{
		//barrier_wait( barrier_var, philosopher_count, philosopher_id );	
		//printf("I'm philosopher %d and my current status is %c\n", philosopher_id, neighbour_status[ philosopher_id ]);
		m_sem_wait( sem_var, neighbour_status, philosopher_id );
		sleep(1);
		m_sem_post( sem_var, neighbour_status, philosopher_id );
		sleep(1);
		//barrier_wait( barrier_var, philosopher_count, philosopher_id );
		//printf("Barrier done %d\n", philosopher_id);
	}
	m_sem_close( sem_var );
	neighbour_status_close( neighbour_status, philosopher_count );
	barrier_var_close( barrier_var );
}
