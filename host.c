#include "lib_sem.h"

void main(int argc, char **argv)
{
	// Get commandline arguments
	int philosophers = atoi( argv[1] );
	//
	
	// Create neighbour status table
	char *neighbour_status = neighbour_status_create( "./neighbour_status_bak.dat", philosophers );
	//
	
	// Create semaphore
	m_sem_t *sem_var = m_sem_create( "./sem_var_bak.dat", philosophers );
	//
	
	// Create barrier
	barrier_t *barrier_var = barrier_var_create( "./barrier_var_bak.dat", philosophers );
	//
	
}
