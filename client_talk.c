// client_talk.c
#include "common.h"
#include "client.h"

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
               printf("str_cli: server terminated prematurely\n");
               exit(1);
            }
         }
         recvline[n] = '\0'; // really don't need this if you use n in
                             // the write function.  If you want to use
                             // strlen, then you need to put put in a
                             // NULL char since the read function does not.
         // write(fileno(stdout), recvline, n);
         printf("%s\n", recvline);
         if (strcmp(recvline, "exit") == 0)
            return;
      }

      if (FD_ISSET(fileno(fp), &rset))
      {
         if ((n = read(fileno(fp), sendline, MAXLINE)) == 0)
         {
            stdineof = 1;
            shutdown(sockfd, SHUT_WR); // end FIN
            FD_CLR(fileno(fp), &rset);
         }
         else
         {
            sendline[n] = '\0'; // really don't need this if you use n in
                                // the write function.  If you want to use
                                // strlen, then you need to put put in a
                                // NULL char since the read function does not.
            write(sockfd, sendline, n);
         }
      }
   }
}
