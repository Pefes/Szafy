#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


void initialize( int *argc, char ***argv )
{
	int tid, size;
	MPI_Status status;

	MPI_Init( argc, argv );

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &tid );
	printf( "My id is %d of %d\n", tid, size );
	
	I = size;
	W = atoi( (*argv)[1] );
	P = atoi( (*argv)[2] );
	srand( time(0) + tid );
	
	//create thread for communication purposes
	pthread_create( &communicationThreadId, NULL, communicationThread, (void *) 0 );
	
	//mutex init
	stateMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	lamportMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
}

void finalize()
{
	pthread_mutex_destroy( &stateMutex );
	pthread_mutex_destroy( &lamportMutex );
	pthread_join( communicationThreadId, NULL );
	MPI_Finalize();
}

int getRandomNumberOfRooms( int P )
{
	return rand() % P + 1;
}

void incrementLamport( int lamportIn )
{
	pthread_mutex_lock( &lamportMutex );
	
	lamport = max( lamportIn, lamport ) + 1;
	
	pthread_mutex_unlock( &lamportMutex );
}

int max( int numberA, int numberB )
{
	return ( numberA > numberB ) ? numberA : numberB;
}