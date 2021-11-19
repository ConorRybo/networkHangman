//server.h
void str_echo(int sockfd);
void sig_child(int signc);
void str_cli(int sockfd,int cin, int cout);

struct client_type {
   int cp[2];
   int pc[2];
   int id;
};

