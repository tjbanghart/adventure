/* BUILDROOMS.C
 *  OS1 - W20: Oregon State University
 *  Thomas Banghart
 *  Friday, Febuary 7th 2020
 *  
 *  buildrooms.c creates a directory named "banghart.room.<pid>" and populates it with 7 "room" files that are to be used with "adventure.c". Each time the program is run a different series
 *  connections and names are generated.
 */

#include "buildrooms.h"

//******** MAIN *********//
int main()
{
    time_t t;
    long size;
    char* dirBuf;

    srand((unsigned) time(&t)); //seed rand() with time

    size = pathconf(".", _PC_PATH_MAX); //set size using pathconfig settings for os
    dirBuf = (char *) malloc((size_t)size); assert(dirBuf != 0);

    initRooms(); //Initialize global rooms array
    
    makeDir(dirBuf, size); //Make directory 
    
    while (isGraphFull() == 0) // Create all connections in graph
    {
        addRandomConnection();
    }
   
    writeRoomFiles(dirBuf); //Write files

    cleanUp(dirBuf);
    return 0;
}


/* isGraphFull()
 * ----------------------------
 *  Determines if graph is "full".
 *  Graph is full when all connections have 3 to 6 outbound connections
 * 
 *   Args:
 *      void
 *   returns: (int) 1 if graph is full 0 if not
 * ----------------------------
 */
int isGraphFull()
{
    int i;
    for(i = 0; i < NUM_OF_ROOMS; i++){
        if(roomsArray[i]->numOutboundConnections < 3 || roomsArray[i]->numOutboundConnections > 6)
        {
            return 0;
        }
    }
    return 1;
    
}


/* getRandomRoom
 * ----------------------------
 * Grabs random room from global array of rooms 
 * 
 *   Args:
 *      void
 *   returns: (struct room*) pointer to randomly selected room
 * ----------------------------
 */
struct room* getRandomRoom()
{ 
    struct room *roomToReturn = roomsArray[rand() % NUM_OF_ROOMS];
    return roomToReturn;   
}


/* canAddConnectionFrom
 * ----------------------------
 *  Checks if a connection can be added from a room
 * 
 *   Args:
 *      x: pointer to room
 *   returns: (int) 1 if a connection can be added from a room, 0 otherwise
 * ----------------------------
 */
int canAddConnectionFrom(struct room *x)
{
    assert(x != 0);
    if(x->numOutboundConnections < 6)
    {
        return 1;
    }
    return 0;
}


/* connectionAlreadyExists
 * ----------------------------
 *  Checks if a connection already exists between two rooms
 * 
 *   Args:
 *      x: pointer to room
 *   returns: (int) 1 if a connection can be added from a room, 0 otherwise
 * ----------------------------
 */
int connectionAlreadyExists(struct room *x, struct room *y)
{
    assert(x != 0 && y != 0);
    int i;
    for(i = 0; i < x->numOutboundConnections; i++)
    {
        if(x->outboundConnections[i]->id == y->id) 
        { 
            return 1; 
        }
    }
    return 0;
}


/* connectRoom
 * ----------------------------
 *  Connects two rooms together 
 * 
 *   Args:
 *      x: pointer to room to add connection from
 *      y: pointer to add connetion to
 *   returns: void
 * ----------------------------
 */
void connectRoom(struct room *x, struct room *y)
{
    assert(x != 0 && y != 0);
    x->outboundConnections[x->numOutboundConnections] = y;
    x->numOutboundConnections++;
    return;

}


/* isSameRoom
 * ----------------------------
 *  Checks if two rooms are the same
 * 
 *   Args:
 *      x: pointer to room 
 *      y: pointer to room
 *   returns: (int) 1 if both rooms are the same, 0 otherwise
 * ----------------------------
 */
int isSameRoom(struct room *x, struct room *y)
{
    assert(x != 0 && y != 0);
    if(x->id == y->id)
    {
        return 1;
    }
    return 0;
}


/* addRandomConnection
 * ----------------------------
 *  Adds two random rooms together, provided that they both 
 *  are able to be connected. Pulls rooms from global array

 *   Args:
 *      void
 *   returns: (int) 1 if both rooms are the same, 0 otherwise
 * ----------------------------
 */
void addRandomConnection()
{
    struct room *x; // Maybe a struct, maybe global arrays of ints
    struct room *y;

    x = getRandomRoom(); 
    while (1)
    {
        x = getRandomRoom();

        if (canAddConnectionFrom(x) == 1)
            break;
    }

    do
    {
        y = getRandomRoom();
    } while (canAddConnectionFrom(y) == 0 || isSameRoom(x, y) == 1 || connectionAlreadyExists(x, y) == 1);

    connectRoom(x, y); 
    connectRoom(y, x); 
}


/* isRoomAlreadyAdded
 * ----------------------------
 *  Determines if randomly selected room has already been added to global rooms array.
 *  If room has already been added, memory for newRoom is freed.
 *   Args:
 *      curNumRooms: current number of rooms in global array
 *      newRoom: room to be potentially added to array
 *   returns: (int) 1 if room is already added to global array, 0 otherwise
 * ----------------------------
 */
int isRoomAlreadyAdded(int curNumOfRooms, struct room *newRoom)
{
    assert(newRoom != 0);
    int i;
    for(i = 0; i < curNumOfRooms; i++)
    {
        if(isSameRoom(newRoom, roomsArray[i]) == 1)
        { 
            free(newRoom); 
            return 1;
        }
    }
    return 0;
}


/* initRooms
 * ----------------------------
 *  Initializes global rooms array to be populated.
 *   Args:
 *      void
 *   returns: void
 * ----------------------------
 */
void initRooms()
{   
    int i = 0;
    int randNum;
    while(i < NUM_OF_ROOMS) 
    {
        randNum = (rand()%10); 
        struct room* newRoom = malloc(sizeof(struct room)); assert(newRoom != 0); //make new room
        newRoom->id = randNum; //assign ID for quick checking 

        if(isRoomAlreadyAdded(i, newRoom)) //check if that room has already been added
        {
            continue; //redo loop if it already exists
        }
        
        memset(newRoom->name, '\0', sizeof(char)*((sizeof(newRoom->name)/sizeof(char)))); //clear name buffer
        strcpy(newRoom->name, ROOM_NAMES[randNum]);  //Copy random room name from global array into name

        memset(newRoom->type, '\0', sizeof(char)*((sizeof(newRoom->type)/sizeof(char)))); //clear room type buffer
        if(i == 0)
        { 
            strcpy(newRoom->type, "START_ROOM"); //since room order is always random we can assign the first room the "start" type
        }
        else if(i == NUM_OF_ROOMS-1) 
        { 
            strcpy(newRoom->type, "END_ROOM"); //give the last room "end" type
        }
        else 
        { 
            strcpy(newRoom->type, "MID_ROOM"); //all others recive "mid" type
        } 

        newRoom->numOutboundConnections = 0;
        roomsArray[i] = newRoom; // assign next open spot in global array the new room

        i++;
    }
}


/* makeDir
 * ----------------------------
 *  Creates subdirectory in current directory to write room files in
 *   Args:
 *     dirBuf: buffer used to hold path to current directory
 *     size: long to set potential size of current directory string
 *   returns: void
 * ----------------------------
 */
char* makeDir(char* dirBuf, long size)
{
    int newDir;
    char *curDir;

    curDir = getcwd(dirBuf, (size_t)size); //Allocate space for directory buffer and call getcwd() to get the current dir.

    pid_t pid = getpid(); //Get the current pid for the program and convert to char* using sprintf()
    char* pidChar = malloc(16*sizeof(char)); assert(pidChar != 0);
    sprintf(pidChar, "%d", pid);
    
    strcat(curDir, "/banghart.rooms."); //Concatenate the directory and filename 
    strcat(curDir, pidChar);

    newDir = mkdir(curDir, 0755); //Make new directory, throw error if it fails
    if(newDir < 0)
    {
        printf("New directory could not be written\n");
        exit(1);
    }
  
    free(pidChar); //clean up and return
    return curDir;
}


/* writeRoomFiles
 * ----------------------------
 *  Writes contents of global 
 *   Args:
 *     dirBuf: buffer used to hold path to current directory
 *     size: long to set potential size of current directory string
 *   returns: void
 * ----------------------------
 */
void writeRoomFiles(char* roomDir)
{
    int j, i = 0;
    char* fileName = calloc(strlen(roomDir), sizeof(char)*ROOM_NAME_LENGTH); assert(fileName != 0);
    
    while(i < NUM_OF_ROOMS)
    {
        FILE* fptr;
        strcpy(fileName, roomDir);
        strcat(fileName, "/");
        strcat(fileName, roomsArray[i]->name);

        fptr = fopen(fileName, "w");
        fprintf(fptr, "ROOM NAME: %s\n", roomsArray[i]->name);
        for(j = 0; j < roomsArray[i]->numOutboundConnections; j++)
        {
            fprintf(fptr, "CONNECTION %d: %s\n", j+1, roomsArray[i]->outboundConnections[j]->name);
        }
        fprintf(fptr, "ROOM TYPE: %s\n", roomsArray[i]->type);

        i++;
        fclose(fptr);
    }
 
    free(fileName);
    return;
}


/* cleanUp
 * ----------------------------
 *  Frees the global rooms array and memory allocated for current directory buffer
 *   Args:
 *      dirBuf: pointer to buffer used to hold 
 *   returns: void
 * ----------------------------
 */
void cleanUp(char* dirBuf)
{
    int i;
    for(i = 0; i < NUM_OF_ROOMS; i++)
    {
        free(roomsArray[i]);
    }
    free(dirBuf);
    return;
}
