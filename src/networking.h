#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> //for close, sleep
#include <stdlib.h> //for malloc

#define SOCKET int
#define SOCKADDR_IN struct sockaddr_in
#define SOCKADDR struct sockaddr
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

//defines socket structure
typedef struct {
	SOCKET s;
	SOCKADDR_IN sa;
} udp_sock_struct_t;


int udp_recv(udp_sock_struct_t uss, char *buff, int bufflen);
int udp_send(udp_sock_struct_t uss, char *buff, int bufflen);

int setupReceiveSocket(udp_sock_struct_t* sock, const char* addr, short int port);
int setupTransmitSocket(udp_sock_struct_t* sock, const char* addr, short int port);
int setupSpecificTransmitSocket(udp_sock_struct_t* sock,  const char* addr,  short int port,
								udp_sock_struct_t* sock2, const char* addr2, short int port2);
