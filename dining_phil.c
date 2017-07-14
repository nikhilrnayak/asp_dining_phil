#include "lib_sem.h"


// Function Prototype Start

void philosopher_create( int, int, int );

// Function Prototype End




void main( int argc, char **argv )
{
	char philosopher_s[10], M_s[10];
	// Get the input arguments
	
	int philosophers = atoi( argv[1] );	// Get the number of philosophers
	//int forks = philosophers;		// Number of forks is equal to the number of philosophers
	int M = atoi( argv[2] );		// Number of times Philosopher loops
	
	// End of getting command line arguments
	
	// fork and create a host. It's the hosts task to set up the semaphore
	
	// if number of philosophers is less than 2 then exit
	if( philosophers < 2 )
	{
		printf("Only one philosopher at the table, there should be at least two so that two forks are there to eat. Exiting!!!!\n");
		exit(0);
	}
	//
	
	sprintf( philosopher_s, "%d", philosophers );
	sprintf( M_s, "%d", M );

	pid_t pid_host = fork();
	
	if( pid_host == 0 )
	{
		// This is the host section of the code
		if( execlp( "./host", "host", philosopher_s, NULL ) == -1 )
			printf("Error execlp host\n");
	}
	else
	{
		// This is the parent section of the code 
		// create as many childrens as philosophers
		wait( pid_host );
		philosopher_create( philosophers, philosophers, M );

	}
}

void philosopher_create( int arg_phil_count, int philosophers, int M )
{
	char philosopher_s[10], M_s[10], arg_phil_count_s[10];
	sprintf( philosopher_s, "%d", philosophers );
	sprintf( M_s, "%d", M );
	sprintf( arg_phil_count_s, "%d", arg_phil_count - 1 );
	if( arg_phil_count > 0 )
	{
		pid_t pid_phil = fork();
		if( pid_phil == 0 )
		{
			if( execlp("./philosopher", "philosopher", M_s, philosopher_s, arg_phil_count_s,  NULL ) == -1 )
				printf("Error execlp phil\n");
		}
		else
		{
			philosopher_create( arg_phil_count - 1, philosophers, M );
			wait( pid_phil );
		}
	}
}
