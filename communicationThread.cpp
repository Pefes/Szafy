#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


void *communicationThread( void *ptr )
{
	pthread_detach( pthread_self() );
	MPI_Status status;
	
	int message[2];
	int messageTag, messageSender, messageLamport, messageValue;
	
	while ( state != FINISH )
	{
		//get the message
		MPI_Recv( message, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
		
		//asign values from received message
		messageTag = status.MPI_TAG;
		messageSender = status.MPI_SOURCE;
		messageLamport = message[0];
		messageValue = message[1];
		
		incrementLamport( messageLamport );
		
		if ( messageTag == REQP )
		{
			if ( state == WAIT_FOR_ACKP )
			{
				//printf( "[%d][%d] %d | %d:", threadId, messageSender, lastLamportREQP, messageLamport );
				if ( isMyLamportLower(messageLamport, messageSender) )
				{
					waitingForRoomPush( messageSender, messageValue );
					//printf(" lower...\n");
				}
				else 
				{
					sendMessageForSingleThread( ACKP, messageSender );
					agreedForRoomPush( messageSender, messageValue );
					//printf(" higher...\n");
				}
			}
			else
			{
				sendMessageForSingleThread( ACKP, messageSender );
				agreedForRoomPush( messageSender, messageValue );
			}
		}
		else if ( messageTag == ACKP )
		{
			incrementCounterACKP( messageSender );
		}
	}

	
	pthread_exit( NULL );
}