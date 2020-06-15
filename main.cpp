#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


int I, W, P, state, numberOfRooms, lamport, threadId, lastLamportREQP, lastLamportREQW;
int counterACKP, counterACKW;
vector < vector<int> > waitingForRoom, agreedForRoom, previousAgreedForRoom;
vector <int> waitingForLift, agreedForLift, previousAgreedForLift;
pthread_t communicationThreadId;
pthread_mutex_t stateMutex, lamportMutex;
pthread_mutex_t waitingForRoomMutex, waitingForLiftMutex, agreedForRoomMutex, agreedForLiftMutex, previousAgreedForRoomMutex, previousAgreedForLiftMutex;


int main( int argc, char **argv )
{
	initialize( &argc, &argv );
	
	//main loop
	while ( state != FINISH )
	{
		if ( state == START )
		{
			numberOfRooms = getRandomNumberOfRooms( P );
			setState( SENDING_REQP );
		}
		if ( state == SENDING_REQP )
		{
			sendMessageForAll( REQP );
			setState( WAIT_FOR_ACKP );
		}
		if ( state == WAIT_FOR_ACKP )
		{
			while ( 1 )
			{
				if ( gotEnoughACKP() && isEnoughFreeRooms() )
				{
					printf( "[%d] (%d) Got access to rooms...\n", threadId, lamport );
					sendACKPForAllWaitingForRoom();
					moveWaitingForRoomToAgreedForRoom();
					setState( SENDING_REQW );
					break;
				}
			}
		}
		if ( state == SENDING_REQW )
		{
			sendMessageForAll( REQW );
			setState( WAIT_FOR_ACKW );
		}
		if ( state == WAIT_FOR_ACKW )
		{
			while ( 1 )
			{
				if ( gotEnoughACKW() && isEnoughFreeLifts() )
				{
					printf( "[%d] (%d) Got access to lift...\n", threadId, lamport );
					sendACKWForAllWaitingForLift();
					moveWaitingForLiftToAgreedForLift();
					setState( USE_LIFT );
					break;
				}
			}
		}
		if ( state == USE_LIFT )
		{
			useLift();
			sendMessageForAll( RELW );
			moveAgreedForLiftToPreviousAgreedForLift();
			counterACKW = 0;
			setState( USE_ROOMS );
		}
		if ( state == USE_ROOMS )
		{
			useRooms();
			setState( SENDING_REQW_BACKWARDS );
		}
		if ( state == SENDING_REQW_BACKWARDS )
		{
			sendMessageForAll( REQW );
			setState( WAIT_FOR_ACKW_BACKWARDS );
		}
		if ( state == WAIT_FOR_ACKW_BACKWARDS )
		{
			while ( 1 )
			{
				if ( gotEnoughACKW() && isEnoughFreeLifts() )
				{
					printf( "[%d] (%d) Got access to lift backwards...\n", threadId, lamport );
					sendACKWForAllWaitingForLift();
					moveWaitingForLiftToAgreedForLift();
					setState( USE_LIFT_BACKWARDS );
					break;
				}
			}
		}
		if ( state == USE_LIFT_BACKWARDS )
		{
			useLift();
			sendMessageForAll( RELW );
			moveAgreedForLiftToPreviousAgreedForLift();
			sendMessageForAll( RELP );
			moveAgreedForRoomToPreviousAgreedForRoom();
			counterACKP = 0;
			counterACKW = 0;
			setState( START );
			printf( "[%d] (%d) I'm out!\n", threadId, lamport );
		}
	}
	
	finalize();
}
