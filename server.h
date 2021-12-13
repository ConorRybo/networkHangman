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
void communicate(int maxi, struct gametype *currentGame, char buff[], fd_set *rset, fd_set *allset, int nready);
void pickCli(bool *tmc, struct gametype *currentGame, struct sockaddr_in *cli_addr, socklen_t *cli_len, int *listenfd, int *connfd, int *i);
void manFDs(int maxfd, int maxi, int listenfd, fd_set *allset, struct gametype *currentGame);
void sockSetup(struct hostent *gethostbyname(), struct hostent *host, char serverHostName[], int port, struct sockaddr_in *serv_addr, int *listenfd);
void childOps(struct gametype *currentGame, int *listenfd, bool tmc, int i);
void parentOps(struct gametype *currentGame, int *connfd, int maxfd, int maxi, int i, fd_set *allset);
void mes2par(int sockfd, int stdineof, int coutfd);
void mes2cli(int sockfd, int stdineof, int cinfd, fd_set *rset);
void str_echo(int sockfd);
void sig_child(int signc);
void str_cli(int sockfd, int cin, int cout);
void start(struct gametype *gameName);
