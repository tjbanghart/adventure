#ifndef BUILDROOMS_H
#define BUILDROOMS_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define ROOM_TYPE_LENGTH 10 + 1
#define ROOM_NAME_LENGTH 8 + 1
#define NUM_OF_ROOMS 7

const char ROOM_NAMES[10][8] = {
    "Gold",
    "Silver",
    "Crimson",
    "Teal",
    "Magenta",
    "Purple",
    "Red",
    "Blue",
    "Green",
    "Orange"
};

struct room 
{
    int id;
    char name[ROOM_NAME_LENGTH];
    int numOutboundConnections;
    char type[ROOM_TYPE_LENGTH];
    struct room* outboundConnections[6];
};

struct room* roomsArray[NUM_OF_ROOMS]; //Global rooms array


//******** FUNCTION PROTOTYPES *********//
//Boolean types
int isGraphFull();
int canAddConnectionFrom(struct room *x);
int connectionAlreadyExists(struct room *x, struct room *y);
int isSameRoom(struct room *x, struct room *y);
int isRoomAlreadyAdded(int curNumOfRooms, struct room *newRoom);

//Global rooms array
struct room* getRandomRoom();
void addRandomConnection();
void initRooms();

//Directory and file
char* makeDir(char* dirBuf, long size);
void writeRoomFiles(char* roomDir);

void cleanUp(char* dirBuf);
//************************************//

#endif
