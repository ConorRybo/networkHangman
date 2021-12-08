// Conor Rybacki
// HangMe pls

// client.c
#include "common.h"
#include "client.h"

int main(int argc, char **argv)
{
   int sockfd, n;
   struct sockaddr_in servaddr;

   signal(SIGINT, CatchInterrupt);

   if (argc != 4)
   { // checking for proper usage
      printf("Usage: a.out <IPaddress> <PortNum> <Username>\n");
      exit(1);
   }

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   { // creating and error checking socket with appropriate variables
      printf("socket error\n");
      exit(2);
   }

   bzero(&servaddr, sizeof(servaddr)); // clearing out the memory space of servaddr completely
   servaddr.sin_family = AF_INET;      // IPV4
   // gets the SERV_PORT from the 'common.h' file
   servaddr.sin_port = htons(SERV_PORT); /*daytime server - normally 13*/ // converts from hostbyte order to network byte order
   /*
      int inet_pton(int af, const char *restrict src, void *restrict dst);
      DESCRIPTION         top
            This function converts the character string src into a network
            address structure in the af address family, then copies the
            network address structure to dst.  The af argument must be either
            AF_INET or AF_INET6.  dst is written in network byte order.
   */
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) // calling functions and error checking
      printf("inet_pton error for %s\n", argv[1]);
   /*
   int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);
      DESCRIPTION         top
            The connect() system call connects the socket referred to by the
            file descriptor sockfd to the address specified by addr.  The
            addrlen argument specifies the size of addr.  The format of the
            address in addr is determined by the address space of the socket
            sockfd; see socket(2) for further details.
   */
   if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) // connecting and error checking
      printf("connect error\n");

   str_cli(stdin, sockfd); // calling main controlling function
   exit(0);
}

void procMes(char *message)
{
   // main purpose really is to process message
   // that was passed from the server
   char bhold[100];            // hold the blanks
   int size = strlen(message); // get size of message
   char guess = 'x';           // logic control variable
   int m = 0;
   // we know if last is 'B' do specific thing
   if (size < 1) // need to avoid seg fault
   {
      guess = 'x'; // just a dummy loop control variable
   }
   else // actually process the last character now
   {
      guess = tolower(message[size - 1]);
   }

   // start conditionals
   if (guess == 'b') // start the blank logic
   {
      for (int x = 0; x < size - 1; x++) // process up to the last spot but stop before
      {
         m = (m * 10) + atoi(message[x]);
      }
      manBlank(m); // call the blank manager function
   }
   else if (guess == 'g')
   {
      ourGuess(); // WHAT DO I SEND THIS THING DUDE???
      // i dont thing anything
   }
   else if (isalpha(guess))
   {
      if (tolower(message[0]) == 'c')
      {
         handleCorrect(bhold);
      }
      else if (tolower(message[0]) == 'i')
      {
         handleWrong(bhold);
      }
      else
      {
         printf("Entry not recognized : ignoring...\n");
      }
   }
   else
   {
      printf("Entry not recognized : ignoring...\n");
   }
   // FLUSH THE INPUT STREAM BEFORE READING FROM CMD LINE
}