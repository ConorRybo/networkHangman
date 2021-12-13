// server_talk.c
#include "common.h"
#include "server.h"

void str_cli(int sockfd, int cinfd, int coutfd)
{
   char sendline[MAXLINE];
   char recvline[MAXLINE];
   bool done = false;
   int maxfdp1;
   int stdineof;
   int n;
   fd_set rset;

   stdineof = 0;
   FD_ZERO(&rset);
   printf("Child ready - sockfd:%d  cinfd:%d coutfd:%d\n", sockfd, cinfd, coutfd);
   sendline[0] = '\0';
   for (;;)
   {
      if (stdineof == 0)
         FD_SET(cinfd, &rset);
      FD_SET(sockfd, &rset);
      maxfdp1 = ((cinfd > sockfd) ? cinfd : sockfd) + 1;

      select(maxfdp1, &rset, NULL, NULL, NULL);

      if (FD_ISSET(sockfd, &rset))
      {
         mes2par(sockfd, stdineof, coutfd);
      }

      if (FD_ISSET(cinfd, &rset))
      {
         mes2cli(sockfd, stdineof, cinfd, &rset);
      }
   }
}

void mes2par(int sockfd, int stdineof, int coutfd)
{
   int n;
   char recvline[MAXLINE];
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
   recvline[n - 1] = '\0'; // really don't need this if you use n in
                           // the write function.  If you want to use
                           // strlen, then you need to put put in a
                           // NULL char since the read function does not.
   write(coutfd, recvline, n);
   printf(".....message %s to parent: fd-%d\n", recvline, coutfd);
}

void mes2cli(int sockfd, int stdineof, int cinfd, fd_set *rset)
{
   int n;
   char sendline[MAXLINE];

   if ((n = read(cinfd, sendline, MAXLINE)) == 0)
   {
      stdineof = 1;
      shutdown(sockfd, SHUT_WR); // end FIN
      FD_CLR(cinfd, rset);
   }
   else
   {
      sendline[n] = '\0'; // really don't need this if you use n in
                          // the write function.  If you want to use
                          // strlen, then you need to put put in a
                          // NULL char since the read function does not.
      write(sockfd, sendline, n);
      printf(".....message %s to client: fd-%d\n", sendline, sockfd);
      if (strcmp(sendline, "exit") == 0)
         return;
   }
}