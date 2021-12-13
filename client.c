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
   char uname[50];
   strcpy(uname, argv[3]);

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   { // creating and error checking socket with appropriate variables
      printf("socket error\n");
      exit(2);
   }

   bzero(&servaddr, sizeof(servaddr)); // clearing out the memory space of servaddr completely
   servaddr.sin_family = AF_INET;      // IPV4
   // gets the SERV_PORT from the 'common.h' file
   servaddr.sin_port = htons(atoi(argv[2])); /*daytime server - normally 13*/ // converts from hostbyte order to network byte order
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
