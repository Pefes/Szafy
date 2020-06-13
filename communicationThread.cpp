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
				if ( isMyLamportLower(messageLamport, messageSender, REQP ) )
				{
					waitingForRoomPush( messageSender, messageValue );
				}
				else 
				{
					sendMessageForSingleThread( ACKP, messageSender );
					//incrementCounter( messageSender, ACKP );
					agreedForRoomPush( messageSender, messageValue );
				}
			}
			else
			{
				sendMessageForSingleThread( ACKP, messageSender );
				//incrementCounter( messageSender, ACKP );
				agreedForRoomPush( messageSender, messageValue );
			}
		}
		else if ( messageTag == ACKP )
		{
			incrementCounter( messageSender, ACKP );
		}
		else if ( messageTag == REQW )
		{
			if ( state == WAIT_FOR_ACKW )
			{
				if ( isMyLamportLower(messageLamport, messageSender, REQW) )
				{
					waitingForLiftPush( messageSender );
				}
				else
				{
					sendMessageForSingleThread( ACKW, messageSender );
					incrementCounter( messageSender, ACKW );
					agreedForLiftPush( messageSender );
				}
			}
			else
			{
				sendMessageForSingleThread( ACKW, messageSender );
				incrementCounter( messageSender, ACKW );
				agreedForLiftPush( messageSender );
			}
		}
		else if ( messageTag == ACKW )
		{
			incrementCounter( messageSender, ACKW );
		}
		else if ( messageTag == RELW )
		{
			removeFromAgreedOrPreviousAgreedForLift( messageSender );
		}
		else if ( messageTag == RELP )
		{
			removeFromAgreedOrPreviousAgreedForRoom( messageSender );
		}
	}

	pthread_exit( NULL );
}