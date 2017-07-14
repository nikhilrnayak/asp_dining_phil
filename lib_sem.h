#ifndef lib_sem_h
#define lib_sem_h
#include "lib_sem.c"

// Fuction Prototypes Start

barrier_t *barrier_var_create( char *, int );

barrier_t *barrier_var_open( char * );

void barrier_wait( barrier_t *, int, int );

void barrier_var_close( barrier_t * );

char *neighbour_status_create( char *, int );

char *neighbour_status_open( char *, int );

void neighbour_status_close( char *, int );

m_sem_t *m_sem_create( char *, int );

m_sem_t *m_sem_open( char * );

void m_sem_post( m_sem_t *, char *, int );

void m_sem_wait( m_sem_t *, char *, int );

void m_sem_close( m_sem_t * );

// Function Prototypes End

#endif
