#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


void *communicationThread( void *ptr )
{
	pthread_detach( pthread_self() );
	MPI_Status status;
	
	int message[2];
	int messageTag, messageSender, messageLamport, messageValue;
	
	while ( 1 )
	{
		//get the message
		MPI_Recv( message, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
		
		//asign values from received message
		messageTag = status.MPI_TAG;
		messageSender = status.MPI_SOURCE;
		messageLamport = message[0];
		messageValue = message[1];
		
		printf( "[%d] Received message from %d with lamport %d and value %d...\n", threadId, messageSender, messageLamport, messageValue );
		
		incrementLamport( messageLamport );
		
		if ( messageTag == REQP )
		{
			if ( state == WAIT_FOR_ACKP )
			{
				if ( isMyLamportLower(messageLamport, messageSender) )
				{
					waitingForRoom.push_back( messageSender );
				}
				else 
				{
					sendMessageForSingleThread( ACKP, messageSender );
				}
			}
			else
			{
				
			}
		}
	}

	
	pthread_exit( NULL );
}