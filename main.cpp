#include "main.h"
#include "utilities.h"
#include "communicationThread.h"


int I, W, P, state, numberOfRooms, lamport, threadId;
pthread_t communicationThreadId;
pthread_mutex_t stateMutex, lamportMutex;


int main( int argc, char **argv )
{
	initialize( &argc, &argv );
	
	//main loop
	while ( 1 )
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
			break;
		}
	}
	
	finalize();

	//int msg[2];
	//MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	//printf("Otrzymalem %d %d od %d\n", msg[0], msg[1], status.MPI_SOURCE);
	//msg[0] = tid;
	//msg[1] = size;
	//MPI_Send( msg, 2, MPI_INT, ROOT, MSG_TAG, MPI_COMM_WORLD );
	//printf("Wyslalem %d %d do %d\n", msg[0], msg[1], ROOT );

}
