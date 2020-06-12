#ifndef MAIN_H
#define MAIN_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define ROOT 0
#define MSG_TAG 100

//process state
#define START 0
#define SENDING_REQP 1

//message type
#define REQP 0

extern int I, W, P;
extern int state;
extern int numberOfRooms;
extern int lamport;

extern pthread_t communicationThreadId;
extern pthread_mutex_t stateMutex;
extern pthread_mutex_t lamportMutex;

int main(int argc, char **argv);


#endif