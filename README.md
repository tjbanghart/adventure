# Adventure

**adventure** is a simple text based adventure game where players must navigate a series of rooms eventually ending up in the "END ROOM". Once a player reaches the "END ROOM" the game is over.  The user is begins in a "start" room named a random color. They are then presented other colors as "POSSIBLE LOCATIONS".

Passing in one of the "POSSIBLE LOCATIONS" to stdin takes the user to that room, etc... If a user passes "time" to stdin then the current date and time is presented and a "currentTime.txt" file is created in the current directory.

This program was made for as part of the course requirements for CS 344: Into To Operating Systems taken at Oregon State Univeristy in Fall 2020. You can read the assignment description [here](/assignment_desc.md).

  

## Setup

- Clone directory
-  `cd` into `adventure` and run `make buildrooms` to compile `buildrooms`
-  `./buildrooms` to create directory of room names for gameplay
-  `make adventure` to create `adventure`
-  `./adventure` to play the game - just follow the prompts!