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
	waitingForRoomMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	waitingForLiftMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	agreedForRoomMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
	agreedForLiftMutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
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
		
	if ( messageType == REQP )
	{
		lastLamportREQP = lamport;
		int message[2] = { lamport, numberOfRooms };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 2, MPI_INT, receiverId, REQP, MPI_COMM_WORLD );
			}
		}
		
		printf( "[%d] (%d) Rooms request: %d...\n", threadId, lamport, numberOfRooms );
	}
	else if ( messageType == REQW )
	{
		lastLamportREQW = lamport;
		int message[2] = { lamport, 1 };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 2, MPI_INT, receiverId, REQW, MPI_COMM_WORLD );
			}
		}
		
		printf( "[%d] (%d) Lift request: 1...\n", threadId, lamport );
	}
	else if ( messageType == RELW )
	{
		int message[2] = { lamport, 0 };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 2, MPI_INT, receiverId, RELW, MPI_COMM_WORLD );
			}
		}
	}
	else if ( messageType == RELP )
	{
		int message[2] = { lamport, 0 };
		
		for ( int receiverId = 0; receiverId < I; receiverId++ )
		{
			if ( threadId != receiverId)
			{
				MPI_Send( message, 2, MPI_INT, receiverId, RELP, MPI_COMM_WORLD );
			}
		}
	}
	
	pthread_mutex_unlock( &lamportMutex );
}

void sendACKPForAllWaitingForRoom()
{
	pthread_mutex_lock( &lamportMutex );
	pthread_mutex_lock( &waitingForRoomMutex );
	
	int message[2] = { lamport, 1 };
	
	for ( int i = 0; i < waitingForRoom.size(); i++ )
	{
		MPI_Send( message, 2, MPI_INT, waitingForRoom[i][0], ACKP, MPI_COMM_WORLD );
	}
	
	pthread_mutex_unlock( &waitingForRoomMutex );
	pthread_mutex_unlock( &lamportMutex );
}

void sendACKWForAllWaitingForLift()
{
	pthread_mutex_lock( &lamportMutex );
	pthread_mutex_lock( &waitingForLiftMutex );
	
	int message[2] = { lamport, 1 };
	
	for ( int i = 0; i < waitingForLift.size(); i++ )
	{
		MPI_Send( message, 2, MPI_INT, waitingForLift[i], ACKW, MPI_COMM_WORLD );
	}
	
	pthread_mutex_unlock( &waitingForLiftMutex );
	pthread_mutex_unlock( &lamportMutex );
}

void sendMessageForSingleThread( int messageType, int receiverId )
{
	pthread_mutex_lock( &lamportMutex );
	
	if ( messageType == ACKP )
	{
		int message[2] = { lamport, 0 };
		MPI_Send( message, 2, MPI_INT, receiverId, ACKP, MPI_COMM_WORLD );
	}
	else if ( messageType == ACKW )
	{
		int message[2] = { lamport, 0 };
		MPI_Send( message, 2, MPI_INT, receiverId, ACKW, MPI_COMM_WORLD );
	}
	
	pthread_mutex_unlock( &lamportMutex );
}

bool isMyLamportLower( int inputLamport, int inputThreadId, int messageType )
{
	int lamportToCheck = 0;
	
	if ( messageType == REQP )
	{
		lamportToCheck = lastLamportREQP;	
	}
	else if ( messageType == REQW )
	{
		lamportToCheck = lastLamportREQW;
	}
	
	if ( lamportToCheck < inputLamport )
	{
		return true;
	}
	else if ( lamportToCheck > inputLamport )
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

void incrementCounter( int messageSender, int counterType )
{
	if ( counterType == ACKP )
	{
		counterACKP++;
	}
	else if ( counterType == ACKW )
	{
		counterACKW++;
	}
}

bool gotEnoughACKP()
{
	int neededAgreements = I - 1;

	if ( counterACKP >= neededAgreements )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isEnoughFreeRooms()
{
	pthread_mutex_lock( &agreedForRoomMutex);
	
	int occupiedRooms = 0;
	
	for ( int i = 0; i < agreedForRoom.size(); i++ )
	{
		occupiedRooms += agreedForRoom[i][1];
	}
	
	pthread_mutex_unlock( &agreedForRoomMutex);
	
	if ( P - occupiedRooms >= numberOfRooms )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool gotEnoughACKW()
{
	int neededAgreements = I - 1;

	if ( counterACKW >= neededAgreements )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isEnoughFreeLifts()
{
	pthread_mutex_lock( &agreedForLiftMutex);
	
	int occupiedLifts = 0;
	
	occupiedLifts += agreedForLift.size();
	
	pthread_mutex_unlock( &agreedForLiftMutex);
	
	if ( occupiedLifts < W )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void waitingForRoomPush( int messageSender, int messageValue )
{
	pthread_mutex_lock( &waitingForRoomMutex);
	waitingForRoom.push_back( vector<int>{ messageSender, messageValue } );
	pthread_mutex_unlock( &waitingForRoomMutex);
}

void agreedForRoomPush( int messageSender, int messageValue )
{
	pthread_mutex_lock( &agreedForRoomMutex);
	agreedForRoom.push_back( vector<int>{ messageSender, messageValue } );
	pthread_mutex_unlock( &agreedForRoomMutex);
}

void waitingForLiftPush( int messageSender )
{
	pthread_mutex_lock( &waitingForLiftMutex);
	waitingForLift.push_back( messageSender );
	pthread_mutex_unlock( &waitingForLiftMutex);
}

void agreedForLiftPush( int messageSender )
{
	pthread_mutex_lock( &agreedForLiftMutex);
	agreedForLift.push_back( messageSender );
	pthread_mutex_unlock( &agreedForLiftMutex);
}

void useLift()
{
	printf( "[%d] (%d) Using lift...\n", threadId, lamport );
	sleep( getRandomTime() );
}

int getRandomTime()
{
	return MIN_SLEEP + rand() % (( MAX_SLEEP + 1) - MIN_SLEEP);
}

void moveWaitingForRoomToAgreedForRoom()
{
	pthread_mutex_lock( &waitingForRoomMutex );
	pthread_mutex_lock( &agreedForRoomMutex );
	
	for ( int i = 0; i < waitingForRoom.size(); i++ )
	{
		agreedForRoom.push_back( vector<int>{ waitingForRoom[i][0], waitingForRoom[i][1] });
	}
	
	waitingForRoom.clear();
	
	pthread_mutex_unlock( &agreedForRoomMutex );
	pthread_mutex_unlock( &waitingForRoomMutex );
}

void moveWaitingForLiftToAgreedForLift()
{
	pthread_mutex_lock( &waitingForLiftMutex );
	pthread_mutex_lock( &agreedForLiftMutex );
	
	for ( int i = 0; i < waitingForLift.size(); i++ )
	{
		agreedForLift.push_back( waitingForLift[i] );
	}
	
	waitingForLift.clear();
	
	pthread_mutex_unlock( &agreedForLiftMutex );
	pthread_mutex_unlock( &waitingForLiftMutex );
}

void removeFromAgreedForLift( int messageSender )
{	
	pthread_mutex_lock( &agreedForLiftMutex );
	
	agreedForLift.erase( remove(agreedForLift.begin(), agreedForLift.end(), messageSender), agreedForLift.end() );
	
	pthread_mutex_unlock( &agreedForLiftMutex );
}

void removeFromAgreedForRoom( int messageSender )
{
	pthread_mutex_lock( &agreedForRoomMutex );
	
	for ( int i = 0; i < agreedForRoom.size(); i++ )
	{
		if ( agreedForRoom[i][0] == messageSender )
		{
			agreedForRoom.erase( agreedForRoom.begin() + i );
			break;
		}
	}
	
	pthread_mutex_unlock( &agreedForRoomMutex );
}

void useRooms()
{
	printf( "[%d] (%d) Using rooms...\n", threadId, lamport );
	sleep( getRandomTime() );
}