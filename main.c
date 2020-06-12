#include "main.h"


int main( int argc, char **argv )
{
	int tid, size;
	MPI_Status status;

	MPI_Init( &argc, &argv );

	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Comm_rank( MPI_COMM_WORLD, &tid );
	printf( "My id is %d of %d\n", tid, size );
	
	I = size;
	W = atoi( argv[0] );
	P = atoi( argv[1] );

	int msg[2];

	//MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	//printf("Otrzymalem %d %d od %d\n", msg[0], msg[1], status.MPI_SOURCE);

	msg[0] = tid;
	msg[1] = size;
	MPI_Send( msg, 2, MPI_INT, ROOT, MSG_TAG, MPI_COMM_WORLD );
	printf("Wyslalem %d %d do %d\n", msg[0], msg[1], ROOT );

	MPI_Finalize();
}
