//client_talk.c
#include"common.h"
#include"client.h"

int max(int a, int b)
{
   return (a>b)?a:b;
}

void CatchInterrupt (int signum)
{
   pid_t my_pid;

   printf("\nReceived an interrupt! About to exit ..%d\n",signum);
   fflush(stdout);
   my_pid = getpid();
   kill(my_pid, SIGKILL);
}


void str_cli(FILE *fp, int sockfd)
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
   printf("Client ready - sockfd:%d\n",sockfd);
   sendline[0] = '\0';
   for(;;) {
      if(stdineof == 0)
         FD_SET(fileno(fp), &rset);
      FD_SET(sockfd, &rset);
      maxfdp1 = max(fileno(fp), sockfd) + 1;

      select(maxfdp1, &rset, NULL, NULL, NULL);

      if(FD_ISSET(sockfd, &rset)) {

         if((n=read(sockfd, recvline, MAXLINE))==0) {
            if(stdineof == 1)
               return;   //normal termination
            else {
               printf("str_cli: server terminated prematurely\n");
               exit(1);
            }
         }
         recvline[n] = '\0';  //really don't need this if you use n in
                              //the write function.  If you want to use
                              //strlen, then you need to put put in a 
                              //NULL char since the read function does not.
         //write(fileno(stdout), recvline, n);
         printf("%s\n",recvline);
         if(strcmp(recvline,"exit")==0)
            return;
      }

      if(FD_ISSET(fileno(fp), &rset)) {
         if((n=read(fileno(fp), sendline, MAXLINE)) == 0) {
            stdineof = 1;
            shutdown(sockfd, SHUT_WR);  //end FIN
            FD_CLR(fileno(fp),&rset);
         } else {
            sendline[n] = '\0'; //really don't need this if you use n in
                                //the write function.  If you want to use
                                //strlen, then you need to put put in a 
                                //NULL char since the read function does not.
            write(sockfd, sendline, n);
         }
      }
   }
}

