#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <vector>
#include <algorithm>

#define MAX_SLEEP 7
#define MIN_SLEEP 5

//process state
#define FINISH -1
#define START 0
#define SENDING_REQP 1
#define WAIT_FOR_ACKP 2
#define SENDING_REQW 3
#define WAIT_FOR_ACKW 4
#define USE_LIFT 5
#define USE_ROOMS 6
#define SENDING_REQW_BACKWARDS 7
#define WAIT_FOR_ACKW_BACKWARDS 8
#define USE_LIFT_BACKWARDS 9

//message type
#define REQP 0
#define ACKP 1
#define REQW 2
#define ACKW 3
#define RELW 4
#define RELP 5

using namespace std;

extern int I, W, P;
extern int threadId;
extern int state;
extern int numberOfRooms;
extern int lamport;
extern int lastLamportREQP, lastLamportREQW;
extern int counterACKP, counterACKW;
extern vector < vector<int> > waitingForRoom;
extern vector < vector<int> > agreedForRoom;
extern vector < vector<int> > previousAgreedForRoom;
extern vector <int> waitingForLift;
extern vector <int> agreedForLift;
extern vector <int> previousAgreedForLift;

extern pthread_t communicationThreadId;
extern pthread_mutex_t stateMutex;
extern pthread_mutex_t lamportMutex;
extern pthread_mutex_t waitingForRoomMutex, waitingForLiftMutex, agreedForRoomMutex, agreedForLiftMutex, previousAgreedForRoomMutex, previousAgreedForLiftMutex;

int main(int argc, char **argv);
