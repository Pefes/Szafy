#include "main.h"
#include "communicationThread.h"


void *communicationThread( void *ptr )
{
	pthread_detach( pthread_self() );
	
	pthread_exit( NULL );
}