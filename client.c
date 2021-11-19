//client.c
#include"common.h"
#include"client.h"

int main(int argc, char **argv)
{
   int sockfd, n;
   struct sockaddr_in servaddr;

   signal(SIGINT, CatchInterrupt);

   if(argc != 2) {
      printf("Usage: a.out <IPaddress>\n");
      exit(1);
   }

   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("socket error\n");
      exit(2);
   }

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(SERV_PORT);  /*daytime server - normally 13*/
   if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
      printf("inet_pton error for %s\n", argv[1]);
   if(connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
      printf("connect error\n");

   str_cli(stdin, sockfd);
   exit(0);
}


