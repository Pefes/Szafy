#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <vector>

#define ROOT 0
#define MSG_TAG 100

//process state
#define START 0
#define SENDING_REQP 1
#define WAIT_FOR_ACKP 2

//message type
#define REQP 0

using namespace std;

extern int I, W, P;
extern int threadId;
extern int state;
extern int numberOfRooms;
extern int lamport;
extern vector <int> waitingForRoom;
extern vector <int> waitingForLift;
extern vector <int> agreedForRoom;
extern vector <int> agreedForLift;
extern vector <int> previousAgreedForRoom;
extern vector <int> previousAgreedForLift;

extern pthread_t communicationThreadId;
extern pthread_mutex_t stateMutex;
extern pthread_mutex_t lamportMutex;

int main(int argc, char **argv);
