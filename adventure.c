
/* ADVENTURE.C
 *  OS1 - W20: Oregon State University
 *  Thomas Banghart
 *  Friday, Febuary 7th 2020
 *  
 *  adventure.c is a simple text based adventure game where players must navigate a series of rooms eventually ending up in the "END ROOM". Once a player reaches the "END ROOM" the game is over. 
 *  This program is to be run after "banghart.buildrooms.c". The user is begins in a "start" room named a random color. They are then presented other colors as "POSSIBLE LOCATIONS".
 *  Passing in one of the "POSSIBLE LOCATIONS" to stdin takes the user to that room, etc... If a user passes "time" to stdin then the current date and time is presented and a "currentTime.txt" file
 *  is created in the current directory.
 */

#include "adventure.h"

//******** MAIN *********//
int main()
{
    int thread_check;

    char startRoomName[ROOM_NAME_LENGTH];
    char mostRecentDirPtr[500];

    memset(startRoomName, '\0', sizeof(startRoomName));
    memset(mostRecentDirPtr, '\0', sizeof(mostRecentDirPtr));

    if (pthread_mutex_init(&lock, NULL) != 0) //initialize mutex for main thread
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    pthread_mutex_lock(&lock); //lock main thread
    thread_check = pthread_create(&tid, NULL, &writeTime, NULL); //create thread for writeTime
    assert(thread_check == 0);

    getMostRecentDir(mostRecentDirPtr); //get most recent directory
    findStartRoom(mostRecentDirPtr, startRoomName); //find the start room
    
    playGame(startRoomName, mostRecentDirPtr); //Play the game

    pthread_mutex_destroy(&lock); //destroy mutex

    return 0;
}



/* generateTime
 * ----------------------------
 *  Gets current date and time, formating per 
 *  assingment instuctions ex "02:37PM, Wednesday, February  5, 2020"
 *  
 *   Args:
 *      timeBuffer: buffer to hold the current time 
 *      bufferSize: size of time buffer
 *   returns: void
 * ----------------------------
 */
void generateTime(char* timeBuffer, int bufferSize)
{
    time_t rawtime;
    struct tm* timeinfo;

    memset(timeBuffer, '\0', bufferSize);
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timeBuffer, bufferSize, "%l:%M%P, %A, %B%e, %G\n", timeinfo);
    return;
}


/* writeTime
 * ----------------------------
 *  Writes current time to "currentTime.txt" in current "adventure" directory
 *  Calls a mutex lock as the function starts.
 *   Args:
 *    void 
 *   returns: void
 * ----------------------------
 */
void* writeTime() //this needs to be a thread
{
    pthread_mutex_lock(&lock); //Call mutex lock

    FILE* fp;
    char timeBuffer[256];

    generateTime(timeBuffer, sizeof(timeBuffer));

    fp = fopen("./currentTime.txt", "w+");
    if (fp == NULL)
    {
        printf("Cannot open file \n"); //print error and exit if operation failed
        exit(1);
    }
    fprintf(fp, "%s", (char*)timeBuffer);
    fclose(fp);

    pthread_mutex_unlock(&lock); //unlock so main thread can lock again
    return NULL; //return Null to keep threading requirement of void* happy

}


/* readTime
 * ----------------------------
 *  Reads time from "currentTime.txt" in current directory
 * 
 *   Args:
 *      void
 *   returns: void
 * ----------------------------
 */
void readTime()
{
    FILE *fp;
    char genBuffer[256];

    fp = fopen("./currentTime.txt", "r");
    if (fp == NULL)
    {
        printf("Cannot open file \n"); //print error and exit if operation failed
        exit(1);
    }

    memset(genBuffer, '\0', sizeof(genBuffer));
    fgets(genBuffer, sizeof(genBuffer), fp);
    printf("\n%s\n", genBuffer);

    fclose(fp);
    return;
}


/* playGame
 * ----------------------------
 *  Prints contents of room file in the proper formatting during gameplay.
 * 
 *   Args:
 *      roomToPrint: room struct to print.
 *   returns: void
 * ----------------------------
 */
void playGame(char *startRoom, char *mostRecentDirPtr)
{
    struct room curRoom;
    int i, thread_check;
    char inputBuf[256]; //input buffer to hold user input
    
    struct dyArray *guessArray = malloc(sizeof(struct dyArray));
    assert(guessArray != 0); //create dynamic array to hold history of user's guesses
    int initalSize = 8; //allow for 8 guesses before resize
    initArray(guessArray, initalSize);

    curRoom = getRoomFromFile(startRoom, mostRecentDirPtr); //set current room to the start room
    printRoom(curRoom); //Print room
    
    do
    {
        printf("WHERE TO? >");
        fgets(inputBuf, sizeof(inputBuf), stdin);
        inputBuf[strlen(inputBuf)-1]='\0';
        
        if(strcmp(inputBuf, "time") == 0)
        {
            pthread_mutex_unlock(&lock);
            pthread_join(tid, NULL);
            pthread_mutex_lock(&lock);
            thread_check = pthread_create(&tid, NULL, &writeTime, NULL); assert(thread_check == 0);
            readTime();
            
            continue;
        }

        else if (!validInput(inputBuf, curRoom)) //validate input, ask again if not valid
        {
            printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            printRoom(curRoom); //Print room
            continue;
        }
        insertArray(guessArray, inputBuf); //insert into array if valid guess
        curRoom = getRoomFromFile(inputBuf, mostRecentDirPtr); //get new current room
        if((strcmp(curRoom.type, "END_ROOM")) == 0) //if it is end room, stop game and output message
        {
            break;
        }
        printf("\n");
        printRoom(curRoom); //Print room

    } while (1);

    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", guessArray->numEle); //show number of steps
    for (i = 0; i < guessArray->numEle; i++)
    {   
        printf("%s\n", guessArray->array[i]); //print steps taken
    }
    freeArray(guessArray);
}


/* printRooms
 * ----------------------------
 *  Prints contents of room file in the proper formatting during gameplay.
 * 
 *   Args:
 *      roomToPrint: room struct to print.
 *   returns: void
 * ----------------------------
 */
void printRoom(struct room roomToPrint)
{
    int i;
    printf("CURRENT LOCATION: %s\n", roomToPrint.name);
    printf("POSSIBLE CONNECTIONS: ");
    for (i = 0; i < roomToPrint.numOutboundConnections; i++)
    {
        if (i == roomToPrint.numOutboundConnections - 1)
        {
            printf("%s.\n", roomToPrint.outboundConnections[i]);
        }
        else
        {
            printf("%s, ", roomToPrint.outboundConnections[i]);
        }
    }
}


/* validInput
 * ----------------------------
 *  Validates user input
 * 
 *   Args:
 *      input: user input
 *      curRoom: structure of current room
 *   returns: (int) 0 if input is invlaid, 1 if valid
 * ----------------------------
 */
int validInput(char* input, struct room curRoom)
{
    int i;
    for(i = 0; i < curRoom.numOutboundConnections; i++)
    {
        if(strcmp(input, curRoom.outboundConnections[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}


/* getMostRecentDir
 * ----------------------------
 *  Searches directory where the program is executed that matches
 *  the prefix "banghart.rooms." Once found, sets the value to 
 *  mostRecentDirPtr passed to it. This directory will be used to play 
 *  the game
 * 
 *   Args:
 *      mostRecentDirPtr: char buffer to hold the most recent directory with "banghart.rooms." prefix.
 *   returns: void 
 * -----------------------------
 */
void getMostRecentDir(char* mostRecentDirPtr)
{
    int newestDirTime = -1;                       // Modified timestamp of newest subdir examined
    char targetDirPrefix[32] = "banghart.rooms."; // Prefix we're looking for
    char newestDirName[256];                      // Holds the name of the newest dir that contains prefix
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR *dirToCheck;           // Holds the directory we're starting in
    struct dirent *fileInDir;  // Holds the current subdir of the starting dir
    struct stat dirAttributes; // Holds information we've gained about subdir

    dirToCheck = opendir("."); // Open up the directory this program was run in

    if (dirToCheck > (void*)0) // Make sure the current directory could be opened
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
            {
                stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

                if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
                {
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                }
            }
        }
    }
    closedir(dirToCheck); // Close the directory we opened
    strcpy(mostRecentDirPtr, newestDirName);
}


/* findStartRoom
 * ----------------------------
 *  Searches files in mostRecentDirPtr argument for file containing 
 *  "ROOM TYPE: START_ROOM". There should always be exactly one file 
 *  with that term. Once found, the startingRoomBuf is set to the file name.
 * 
 *   Args:
 *      mostRecentDirPtr: char buffer to hold the most recent directory with "banghart.rooms." prefix.
 *      startingRoomBuf: char buffer to hold the file name with line == "ROOM TYPE: START_ROOM"
 *   returns: void 
 * -----------------------------
 */
void findStartRoom(char* mostRecentDirPtr, char* startingRoomBuf)
{
    DIR* dirToCheck; 
    FILE *fp;
    struct dirent *fileInDir;
    char lineBuf[256];
    char genBuffer[256] = ""; //initialize buffer to empty string so sprintf() is happy

    sprintf(genBuffer, "./%s", mostRecentDirPtr); //make genBuffer = ./<name_of_most_recent_directory> where ./ = current directory.
    dirToCheck = opendir(genBuffer); //open ./<name_of_most_recent_directory> 

    if(dirToCheck > (void*)0) //check if directory could be opened
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in directory
        {
            if( (strcmp(fileInDir->d_name, ".") == 0) || (strcmp(fileInDir->d_name, "..") == 0)  ) //if the file name is "." or ".." ignore and continue loop
            {
                continue;
            }
            memset(genBuffer, '\0', sizeof(genBuffer)); //reset genBuffer
            sprintf(genBuffer, "./%s/%s", mostRecentDirPtr, fileInDir->d_name); //make genBuffer == full path to file to open from current directory

            fp = fopen(genBuffer, "r"); //open file to read
            if (fp == NULL)
            {
                printf("Cannot open file \n"); //print error and exit if operation failed
                exit(1);
            }
            while ((fgets(lineBuf, 256, fp)) != NULL) //if opened file, begin reading line by line
            {
                if (strcmp(lineBuf, "ROOM TYPE: START_ROOM\n") == 0) //If a line matches target string
                {
                    memset(startingRoomBuf, '\0', strlen(startingRoomBuf)); //clear startingRoomBuf
                    strcpy(startingRoomBuf, fileInDir->d_name); //copy current file name to startingRoomBuf
                    fclose(fp); //close file
                    closedir(dirToCheck); //close directory
                    return; 
                }
            }
            fclose(fp); //if no line matches target, close file and move to next
        }
    closedir(dirToCheck); //if no match found close the directory
    }

}


/* numOfLines
 * ----------------------------
 *  Counts occurance of new line charecters in file.
 * 
 *   Args:
 *      fp: pointer to file to read.
 *   returns: (int) the number of new line charecters in file
 * -----------------------------
 */
int numOfLines(FILE *fp)
{
    char c;
    int numLines = 0;
    while( (c = fgetc(fp)) != EOF)
    {
        if(c == '\n') 
        {
            numLines++;
        }
    }
    rewind(fp); //since pointer is now at end of file, rewind() so it's at the begining
    return numLines;
}


/* initRoom
 * ----------------------------
 *  Initializes room structures that hold contents of room files during 
 *  game play. 
 * 
 *   Args:
 *      room: pointer to room struct to initialize 
 *   returns: void
 * -----------------------------
 */
void initRoom(struct room *room)
{
    int i;
    memset(room->name, '\0', ROOM_NAME_LENGTH * sizeof(char)); //clear room name
    memset(room->type, '\0', ROOM_TYPE_LENGTH * sizeof(char)); //clear room type
    for(i = 0; i < 6; i++)
    {
        memset(room->outboundConnections[i], '\0', ROOM_NAME_LENGTH * sizeof(char)); //clear array of outbound connections
    }
    room->numOutboundConnections = 0;
    return;
}


/* getRoomFromFile
 * ----------------------------
 *  Reads room file and translates its contents into a room struct
 *  to be used during game play
 * 
 *   Args:
 *      roomToPrint: pointer to room file name to read from
 *      mostRecentDirPtr: pointer to most recent directory
 *   returns: (struct room) current room the player is located.
 * -----------------------------
 */
struct room getRoomFromFile(char* roomToPrint, char* mostRecentDirPtr)
{
    FILE *fp; 
    char *ptr;
    int  totalLine, curLine;
    char roomDir[256] = ""; //initialize roomDir to "" so sprintf() is happy
    char lineBuf[500]; //general line buffer to hold contents of line while reading file
    struct room roomFromFile; //struct to return

    initRoom(&roomFromFile); //call initRoom() to ensure the local room struct var is clear of garbage

    memset(roomDir, '\0', sizeof(roomDir)*sizeof(char));  //clear roomDir buffer 
    sprintf(roomDir, "./%s/%s", mostRecentDirPtr, roomToPrint); //make roomDir full path to room file to open

    fp = fopen(roomDir, "r");
    if (fp == NULL)
    {
        printf("Cannot open file \n"); //exit if we can not open the file 
        exit(1);
    }

    totalLine = numOfLines(fp); //get the total number of lines for conditional logic while reading file.
    curLine = 1; 

    while (fgets(lineBuf, 500, fp) != NULL) //fill buffer with file line
    {
        ptr = strchr(lineBuf, ':'); //point ptr to string after ':' char
        ptr += 2; //increment ptr twice to account for the ':' and ' ' chars.
        ptr[strcspn(ptr, "\n")] = 0; //replace the newline char at end of ptr string.
        if(curLine == 1)
        {
            sprintf(roomFromFile.name, "%s", ptr); //first line will always be the room name
        }
        else if(curLine > 1 && curLine < totalLine)
        {
            sprintf(roomFromFile.outboundConnections[roomFromFile.numOutboundConnections], "%s", ptr); //after line 1 to before last line will be connections
            roomFromFile.numOutboundConnections++;
        }
        else
        {
            sprintf(roomFromFile.type, "%s", ptr); //last line will be type
        }
        curLine++;
        memset(lineBuf, '\0', sizeof(lineBuf)*sizeof(char)); //clear lineBuf to get read to fgets() next line
    }
    fclose(fp);
    return roomFromFile;
}


/* initArray
 * ----------------------------
 *   Initializes dynamic array used to track 
 *   the player's route through the game.
 * 
 *  Args:
 *      a: pointer to dyArray to be initialized
 *      initialSize: initialSize of array
 *  Returns: void 
 * -----------------------------
 */
void initArray(struct dyArray* a, int initialSize)
{
    assert(a != 0); //check for valid array pointer
    int i;
    a->array = malloc(sizeof(char*) * initialSize); //allocate a pointer to char* (char array) 
    for(i = 0; i < initialSize; i++)
    {
        a->array[i] = malloc(ROOM_NAME_LENGTH * sizeof(char)); assert(a->array[i] != 0); //allocate space for room name in each index of char array
    }
    a->size = initialSize; 
    a->numEle = 0; 
}


/* insertArray
 * ----------------------------
 *   Adds element to dynamic array. Increases capacity 
 *   if num of elements == size.
 * 
 *  Args:
 *      a: pointer to dyArray to add element to.
 *      eleToEnter: element to add to array
 *   returns: void 
 * ----------------------------
 */
void insertArray(struct dyArray* a, char* eleToEnter)
{
    assert(a != 0 && eleToEnter != 0); //check for valid array pointer and element to enter
    int i;
    if(a->size == a->numEle) //increase size of array if it is full
    {
        a->size = a->size*2;
        a->array = realloc(a->array, sizeof(char *) * a->size); //reallocate memory to copy old values and add length of new size
        for(i = a->numEle; i < a->size; i++)
        {
            a->array[i] = malloc(ROOM_NAME_LENGTH * sizeof(char)); assert(a->array[i] != 0); //from end of old array to size of new -> allocate space for each room name
        }
    }
    strcpy(a->array[a->numEle], eleToEnter); //copy new element in next avalible index
    a->numEle++; 
}


/* freeArray
 * ----------------------------
 *  Frees memory allocated for dynamic array. 
 * 
 *  Args:
 *      a: pointer to dyArray to add element to.
 *  returns: void 
 * ----------------------------
 */
void freeArray(struct dyArray* a)
{
    assert(a != 0); 
    int i;

    for(i = 0; i < a->size; i++)
    {
        free(a->array[i]); //free char* at index
        a->array[i] = 0; //point it to null
    }
    free(a->array); //free char**
    a->array = 0; //point it to null for good measure
    a->size = 0; 
    a->numEle = 0;
    free(a); //free array struct 
    a = 0; //point it to null
}

