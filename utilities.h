void initialize( int *argc, char ***argv );
void finalize();
void setState( int newState );
int getRandomNumberOfRooms( int P );
void incrementLamport( int lamportIn );
int max( int numberA, int numberB );
void sendMessageForAll( int messageType );