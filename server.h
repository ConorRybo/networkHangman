// server.h
struct client_type
{
   int cp[2];
   int pc[2];
   int id;
};

struct gametype
{

   struct client_type players[4]; // hold the players
   int nPlayers;
   bool begun;     // flag to see if the games started yet
   char word[20];  // word to be guessed
   char guess[20]; // holds the guessed characters
   int incor;      // number of incorrect guesses
   int iTurn;      // holds the index of the client whose turn it is
};
void str_echo(int sockfd);
void sig_child(int signc);
void str_cli(int sockfd, int cin, int cout);
void start(struct gametype *gameName);
