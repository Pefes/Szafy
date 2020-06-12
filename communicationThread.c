#include "main.h"
#include "communicationThread.h"


void *communicationThread( void *ptr )
{
	pthread_detach( pthread_self() );
	
	printf("ELO Z DRUGIEGO WĄTKU!\n");
	
	pthread_exit( NULL );
}