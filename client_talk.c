// client_talk.c
#include "common.h"
#include "client.h"
#include <ctype.h>
// function that will return the max of a and b
// called in the str_cli function
int max(int a, int b)
{
   return (a > b) ? a : b;
}

// basically just prints the interrupt and the signal number
// to the user
void CatchInterrupt(int signum)
{
   pid_t my_pid;

   printf("\nReceived an interrupt! About to exit ..%d\n", signum);
   fflush(stdout);
   my_pid = getpid();
   kill(my_pid, SIGKILL);
}

// takes  a file pointer and a socet
void str_cli(FILE *fp, int sockfd)
{
   char sendline[MAXLINE];
   char recvline[MAXLINE];
   bool done = false;
   int maxfdp1;
   int stdineof;
   int n;
   fd_set rset;
   struct gametype myGame; // initialize the game struct to hold information
   myGame.turns = 0;
   myGame.guess = false;
   /*
   Four macros are provided to manipulate the sets. FD_ZERO() clears a set.
   FD_SET() and FD_CLR() respectively add and remove a given file descriptor from a set.
   FD_ISSET() tests to see if a file descriptor is part of the set; this is useful after select() returns.
   */

   stdineof = 0;
   FD_ZERO(&rset); // clears the file descriptor set
   printf("Client ready - sockfd:%d\n", sockfd);
   sendline[0] = '\0';
   for (;;)
   {
      if (stdineof == 0)
         FD_SET(fileno(fp), &rset);
      FD_SET(sockfd, &rset);
      maxfdp1 = max(fileno(fp), sockfd) + 1;

      /*
     select() and pselect() allow a program to monitor multiple file descriptors,
     waiting until one or more of the file descriptors become "ready" for
     some class of I/O operation (e.g., input possible). A file descriptor is considered
     ready if it is possible to perform the corresponding I/O operation (e.g., read(2)) without blocking.
      */
      select(maxfdp1, &rset, NULL, NULL, NULL);

      if (FD_ISSET(sockfd, &rset))
      {

         if ((n = read(sockfd, recvline, MAXLINE)) == 0)
         {
            if (stdineof == 1)
               return; // normal termination
            else
            {
               printf("str_cli: server terminated prematurely; game might be full or in progress\n");
               exit(1);
            }
         }
         // recvline[n] = '\0'; // really don't need this if you use n in
         // the write function.  If you want to use
         // strlen, then you need to put put in a
         // NULL char since the read function does not.
         // write(fileno(stdout), recvline, n);
         // printf("%s\n", recvline);
         procMes(recvline, &myGame); // process the message sent by the
         if (strcmp(recvline, "exit") == 0)
            return;
      }
      // if we have a guess then we get to send a message
      if (myGame.guess)
      {
         printf("It is your turn! Please guess a letter: ");
         if (FD_ISSET(fileno(fp), &rset))
         {
            fflush(fp); // flush stdin so we dont get junk

            if ((n = read(fileno(fp), sendline, MAXLINE)) == 0) // reading from stdin
            {
               stdineof = 1;
               shutdown(sockfd, SHUT_WR); // end FIN
               FD_CLR(fileno(fp), &rset);
            }
            else
            {
               // sendline[n] = '\0'; // really don't need this if you use n in
               // the write function.  If you want to use
               // strlen, then you need to put put in a
               // NULL char since the read function does not.
               write(sockfd, sendline, n);
            }
         }
      }
   }
}

void procMes(char message[], struct gametype *currentGame)
{
   // main purpose really is to process message
   // that was passed from the server
   int size = strlen(message); // get size of message
   char cont = 'x';            // logic control variable
   int m = 0;
   currentGame->turns++; // increment the number of turns
   // we know if last is 'B' do specific thing
   if (size < 1) // need to avoid seg fault
   {
      printf("avoid seg\n");
      cont = 'x'; // just a dummy loop control variabl
   }
   else // actually process the first character
   {
      cont = tolower(message[0]);
   }
   int k;
   // start conditionals
   if (cont >= '1' || cont <= '9') // start the blank logic (only case that starts with a num)
   {
      for (int x = 0; x < size - 1; x++) // process up to the last spot but stop before
      {
         k = message[x];
         m = (m * 10) + k; // gets the actual int number of blanks
      }
      setBlank(m, currentGame); // call the blank manager function
   }
   else if (cont == 'g') // if there is just a G passed its our turn to go
   {
      currentGame->guess = true;
   }
   else if (cont == 'c')
   {
      handleCorrect(currentGame, message); // update blanks and such
   }
   else if (cont == 'i')
   {
      handleWrong(currentGame, message);
   }
   else
   {
      currentGame->turns -= 1; // if a turn actually isnt occuring than just decrement
      printf("%s", message);
   }

   // FLUSH THE INPUT STREAM BEFORE READING FROM CMD LINE
}

void setBlank(int nblank, struct gametype *currentGame)
{
   currentGame->nblank = nblank;
   currentGame->ninc = 0;  // since new game incor starts at zero
   currentGame->turns = 0; // when there are new blanks sent its a new game
   for (int x = 0; x < nblank; x++)
   {
      currentGame->blanks[x] = '_'; // set up the actual blanks
   }
}

void handleCorrect(struct gametype *currentGame, char mes[])
{
   bool mult = false;
   // first look for C#,#,# where the # correspond to the position
   // that the letter needs to be replaced at
   int k = mes[2]; // get the first position to replace
   if (strlen(mes) > 3)
   {
      int x = 2;
      while (true)
      {
         currentGame->blanks[k] = mes[1]; // mess 1 holds the alpha
         x += 2;                          // safety control to avoid seg faulting
         if (x < strlen(mes))             // if we are passed the end of the message
         {
            break;
         }
         k = mes[x]; // get the next position we are replacing at
      }
   }
   else
   {
      currentGame->blanks[k] = mes[1];
   }
}

void handleWrong(struct gametype *currentGame, char mes[])
{
   currentGame->inc[currentGame->ninc] = mes[1]; // put the incorrect char into list
   currentGame->ninc++;                          // increment the number of incorrect
}

void printMan(int lnum)
{
   printf("%d incorrect", lnum);
}

void printBlanks(struct gametype *currentGame)
{
   printf("\n");
   for (int x = 0; x < currentGame->nblank; x++)
   {
      printf("%c ", currentGame->blanks[x]);
   }
   printf("\n");
   printf("\n");
}
