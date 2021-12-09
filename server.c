// server.c
#include "common.h"
#include "server.h"

int main(int argc, char **argv)
{
   struct hostent *gethostbyname();
   char serverHostName[HOSTNAMELEN];
   struct hostent *host;

   int i, j, maxi, maxfd, listenfd, connfd, sockfd;
   struct client_type client[FD_SETSIZE];
   int client_sockID;
   int client_readfd;
   int nready;
   size_t n;
   fd_set rset, allset;
   socklen_t cli_len;
   struct sockaddr_in cli_addr;
   struct sockaddr_in serv_addr;
   char buff[MAXLINE];
   int childpid;
   int port;

   // word to be guessed
   struct gametype currentGame;
   currentGame.begun = false;
   bool tmc = false;

   int perTurn = 0; // this holds whose turn it is
   int cons = 0;

   /* Get the server host name and port number from the command line */
   if (argc == 3)
   {
      strcpy(serverHostName, argv[1]);
      port = atoi(argv[2]);
   }
   else // demonstrates the usage of the executable
   {
      printf("Usage: server <host_name> <port no.>\n");
      exit(0);
   }

   // FD_SETSIZE : /* Maximum number of file descriptors in `fd_set'.  */
   printf("FD_SETSIZE = %d\n", FD_SETSIZE);
   listenfd = socket(AF_INET, SOCK_STREAM, 0); // opens a socket based on the the provided information
   // - AF_INET : IPv4 address family
   // - SOCK_STREAM : TCP

   /* Need to fill-in the internet info */
   host = gethostbyname(serverHostName);
   bzero(&serv_addr, sizeof(serv_addr));                     // erases the bytes in the serv_addr mem space with the size stated
   serv_addr.sin_family = host->h_addrtype;                  // gets the type of the ip address
   serv_addr.sin_port = htons(port);                         /* Arbitrary port number htons puts it into network byte order */
   bcopy(host->h_addr, &serv_addr.sin_addr, host->h_length); // byte order cautious? copy of the host address to the

   /* Advertise listener's name */
   if (bind(listenfd, (SA *)&serv_addr, sizeof(serv_addr)))
   {
      printf("Bind error ... restart\n");
      exit(1);
   };
   // listen() marks the socket referred to by sockfd as a passive
   // socket, that is, as a socket that will be used to accept incoming
   // connection requests using accept(2).
   listen(listenfd, LISTENQ);
   /*
   Four macros are provided to manipulate the sets. FD_ZERO() clears a set.
   FD_SET() and FD_CLR() respectively add and remove a given file descriptor from a set.
   FD_ISSET() tests to see if a file descriptor is part of the set; this is useful after select() returns.
   */
   maxfd = listenfd;
   maxi = -1;
   for (i = 0; i < 4; i++) // goes through and sets all the ids to -1
      currentGame.players[i].id = -1;
   // client[i].id = -1;
   FD_ZERO(&allset);
   FD_SET(listenfd, &allset);

   for (;;)
   {
      rset = allset;
      nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
      /*
       select() allows a program to monitor multiple file descriptors,
       waiting until one or more of the file descriptors become "ready"
       for some class of I/O operation (e.g., input possible).  A file
       descriptor is considered ready if it is possible to perform a
       corresponding I/O operation (e.g., read(2), or a sufficiently
       small write(2)) without blocking.
      */
      if (FD_ISSET(listenfd, &rset))
      {
         cli_len = sizeof(cli_addr);
         connfd = accept(listenfd, (SA *)&cli_addr, &cli_len);

         i = 0; // find an unused client to store the socket id
         // while (client[i].id >= 0 && i < FD_SETSIZE)
         while (currentGame.players[i].id >= 0 && i < FD_SETSIZE)
            i++;
         if (i < 4) // only allow for up to 4 players in the game
         {
            // client[i].id = connfd;
            currentGame.players[i].id = connfd;
            currentGame.nPlayers++;
         }
         else
         {
            printf("Too many clients!\n");
            tmc = true; // drop control flag
         }

         // pipe(client[i].cp);
         // pipe(client[i].pc);
         pipe(currentGame.players[i].cp);
         pipe(currentGame.players[i].pc);
         if ((childpid = fork()) == 0)
         {
            // child
            if (!tmc) // if there isn't already too many clients
            {
               close(listenfd);
               close(currentGame.players[i].cp[0]); // close read side of child to parent
               close(currentGame.players[i].pc[1]); // close write side of parent to chile
               //          sockfd                           cin                        cout
               str_cli(currentGame.players[i].id, currentGame.players[i].pc[0], currentGame.players[i].cp[1]);
               close(currentGame.players[i].cp[1]);
               close(currentGame.players[i].pc[0]);
               // close(client[i].cp[0]);
               // close(client[i].pc[1]);
               // str_cli(client[i].id, client[i].pc[0], client[i].cp[1]);
               // close(client[i].cp[1]);
               // close(client[i].pc[0]);
            }
            // if there too many clients just exit
            printf("exiting child ....\n");
            exit(0);
         }
         else
         {
            // parent
            close(connfd);          // let the child take care of this
            close(client[i].cp[1]); // closes the write side chi to par
            close(client[i].pc[0]); // closes the read side of par to chi
            // but we need to monitor the read side of the port and
            // be able to write to the write side to the child
            FD_SET(client[i].cp[0], &allset); // add the read side of child to parent to the fd set
            if (client[i].cp[0] > maxfd)      // keeping a max file descriptor val
               maxfd = client[i].cp[0];
            if (i > maxi)
               maxi = i;
            if (--nready <= 0)
               continue; // no more readable descriptors
         }
      }
      /*
      for (i = 0; i <= maxi; i++) // for all of the clients
      {
         if ((client[i].id) < 0) // look for a valid client
            continue;
         client_readfd = client[i].cp[0];    // isolate the child to parent read end of the pipe
         if (FD_ISSET(client_readfd, &rset)) // make sure it's in the set
         {
            if ((n = read(client_readfd, buff, MAXLINE)) == 0)
            {
               close(client_readfd);
               close(client[i].pc[1]);
               close(client[i].id);
               FD_CLR(client_readfd, &allset);
               client[i].id = -1;
            }
            else if (strcmp(buff, "exit") == 0)
            {
               write(client[i].pc[1], buff, n);
               close(client_readfd);
               close(client[i].pc[1]);
               close(client[i].id);
               client[i].id = -1;
            }
            else
            {
               buff[n - 1] = '\0';
               n--;
               printf("READ from child %d fd %d: %s\n", i, client_readfd, buff);
               for (j = 0; j <= maxi; j++)
               {
                  if (client[j].id < 0)
                     continue;
                  write(client[j].pc[1], buff, n);
                  printf("Write to child %d fd %d: %s\n", j, client[j].pc[1], buff);
               }
               // printf("WRITE to all: %s\n",buff);
            }
            if (--nready <= 0)
               break;
         }
      }
      */
   }

   return 0;
}

void start(struct gametype *gameName)
{
   strcpy(gameName->word, "pineapple");
   strcpy(gameName->guess, "");
   gameName->incor = 0;
}
