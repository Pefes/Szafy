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
					printf( "[%d] Got acces to rooms...\n", threadId );
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
					printf( "[%d] Got acces to lift...\n", threadId );
					setState( USE_LIFT );
					break;
				}
			}
		}
		if ( state == USE_LIFT )
		{
			
		}
	}
	
	finalize();
}
