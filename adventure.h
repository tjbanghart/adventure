#ifndef ADVENTURE_H
#define ADVENTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

//******** GLOBALS*********//
#define ROOM_NAME_LENGTH 8 + 1
#define ROOM_TYPE_LENGTH 11 + 1

//******** STRUCTS *********//
struct room
{
    char name[ROOM_NAME_LENGTH];
    int  numOutboundConnections;
    char outboundConnections[6][ROOM_NAME_LENGTH];
    char type[ROOM_TYPE_LENGTH];
};

struct dyArray
{
    char** array;
    int   size;
    int   numEle;
};

//******** THREADS *********//
pthread_t tid;
pthread_mutex_t lock;


//******** FUNCTION PROTOTYPES *********//

//Dynamic array
void initArray(struct dyArray* a, int initialSize);
void insertArray(struct dyArray* a, char* eleToEnter);
void freeArray(struct dyArray* a);

//Directory and file search
void getMostRecentDir(char* mostRecentDirPtr);
void findStartRoom(char* mostRecentDirPtr, char* startingRoomBuf);
int numOfLines(FILE *fp);
void initRoom(struct room *room);
struct room getRoomFromFile(char* roomToPrint, char* mostRecentDirPtr);

//Gameplay
void playGame(char *startRoom, char *mostRecentDirPtr);
int validInput(char* input, struct room curRoom);
void printRoom(struct room roomToPrint);
void generateTime(char* timeBuffer, int bufferSize);
void* writeTime();
void readTime();





#endif
