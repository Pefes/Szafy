#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


void initialize( int *argc, char ***argv )
{
	int tid, size;

	MPI_Init( argc, argv );

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &tid );
	//printf( "My id is %d of %d\n", tid, size );
	
	I = size;
	W = atoi( (*argv)[1] );
	P = atoi( (*argv)[2] );
	threadId = tid;
	state = START;
	lamport = 0;
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
	pthread_mutex_lock( &lamportMutex );
	
	lastLamportREQP = lamport;
	
	if ( messageType == REQP )
	{
		int message[2] = { lamport, numberOfRooms };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 2, MPI_INT, receiverId, REQP, MPI_COMM_WORLD );
				printf( "[%d] Message sent to [%d]...\n", threadId, receiverId );
			}
		}
	}
	
	pthread_mutex_unlock( &lamportMutex );
}

void sendMessageForSingleThread( int messageType, int receiverId )
{
	pthread_mutex_lock( &lamportMutex );
	
	if ( messageType == ACKP )
	{
		int message[2] = { lamport, NULL };
		MPI_Send( message, 2, MPI_INT, receiverId, ACKP, MPI_COMM_WORLD );
	}
	
	pthread_mutex_unlock( &lamportMutex );
}

bool isMyLamportLower( int inputLamport, int inputThreadId )
{
	if ( lastLamportREQP < inputLamport )
	{
		return true;
	}
	else if ( lastLamportREQP > inputLamport )
	{
		return false;
	}
	else if ( threadId < inputThreadId )
	{
		return true;
	}
	else
	{
		return false;
	}
}