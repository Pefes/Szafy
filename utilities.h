#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>
#include <stdio.h>


void initialize( int *argc, char ***argv );
void finalize();
int getRandomNumberOfRooms( int P );
void incrementLamport( int lamportIn );
int max( int numberA, int numberB );


#endif