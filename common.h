// common.h
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>

#define MAXLINE 1024
#define LISTENQ 1024
//#define SERV_PORT 48040
#define HOSTNAMELEN 100

//#define FD_SETSIZE 10
#define bool int
#define true 1
#define false 0

#define SA struct sockaddr
