// client.h

void str_cli(FILE *fp, int sockfd);
void CatchInterrupt(int signum);
// holds current information about the game;
struct gametype
{
    int turns;        // counts the number of turns
    bool guess;       // boolean value to hold if its the players turn or not
    char blanks[100]; // hold the blank characters
    int nblank;       // holds the number of blanks in current game
    char inc[100];    // hold the incorrectly guessed characters
    int ninc;         // hold the number of incorrect
};

void procMes(char message[], struct gametype *currentGame);
// MAIN LOGIC MESSAGE HANDLER
// this function is where the message from the server will be passed to procees
// the message coming in from the server
void setBlank(int bnum, struct gametype *currentGame); // manage the blanks to the gamer
void handleCorrect(struct gametype *currentGame, char mes[]);
void handleWrong(struct gametype *currentGame, char mes[]);
void printMan(int lnum);
// this will print the current state of the hangman
// based on lnum := the number of incorrect characters
void printBlanks(struct gametype *currentGame);