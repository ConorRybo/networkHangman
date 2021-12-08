// client.h
void str_cli(FILE *fp, int sockfd);
void CatchInterrupt(int signum);
void procMes(char *message);
// MAIN LOGIC MESSAGE HANDLER
// this function is where the message from the server will be passed to procees
// the message coming in from the server
void manBlank(int bnum); // manage the blanks to the gamer
void handleCorrect(char *blank);
void handleWrong(char *blank);
void ourGuess(); // controls logic for our turn to guess
void printMan(int lnum);
// this will print the current state of the hangman
// based on lnum := the number of incorrect characters
