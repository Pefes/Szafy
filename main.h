#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "utilities.h"
#include "communicationThread.h"

#define ROOT 0
#define MSG_TAG 100

//process state
#define START 0
#define SENDING_REQP 1

//message type
#define REQP 0

int I, W, P;
int state = START;
int numberOfRooms;

pthread_t communicationThreadId;
pthread_mutex_t stateMutex;

int main(int argc, char **argv);