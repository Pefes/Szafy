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
	//printf( "My id is %d of %d\n", tid, size );
	
	I = size;
	W = atoi( (*argv)[1] );
	P = atoi( (*argv)[2] );
	threadId = tid;
	state = START;
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

void setState( int newState )
{
	pthread_mutex_lock( &stateMutex );
	
	state = newState;
	
	pthread_mutex_unlock( &stateMutex );
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

void sendMessageForAll( int messageType )
{
	if ( messageType == REQP )
	{
		int message[3] = { threadId, lamport, numberOfRooms };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 3, MPI_INT, receiverId, REQP, MPI_COMM_WORLD );
				printf( "Message sent from [%d] to [%d]!\n", threadId, receiverId );
			}
		}
	}
}