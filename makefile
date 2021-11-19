# to compile the server
#     make server
#
# to compile the client
#     make client
#
# to start with a clean slate
#     make clean
#
client:		client.o client_talk.o
		cc -o EC client.o client_talk.o 

client.o:	client.c
		cc -c -g client.c

client_talk.o:		client_talk.c
		cc -c -g client_talk.c

server:		server.o server_talk.o
		cc -o ES server.o server_talk.o 

server.o:	server.c
		cc -c -g server.c

server_talk.o:		server_talk.c
		cc -c -g server_talk.c

clean:
		rm *.o EC ES


