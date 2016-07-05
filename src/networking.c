#include "networking.h"

//defines packet receive function
int udp_recv(udp_sock_struct_t uss, char *buff, int bufflen) {
	int sa_size = sizeof(SOCKADDR);
	return recvfrom(uss.s,buff,bufflen,0,(SOCKADDR*)&uss.sa,&sa_size);
}

//defines packet send function
int udp_send(udp_sock_struct_t uss, char *buff, int bufflen) {
	return sendto(uss.s,buff,bufflen,0,(struct sockaddr*)&uss.sa,sizeof(struct sockaddr));
}

//defines create receive socket function
int setupReceiveSocket(udp_sock_struct_t* sock, const char* addr, short int port ) {
	int ret;
	int first_octet;
	sock->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock->s == INVALID_SOCKET) {
		printf("Error creating UDP receive socket\n");
		sleep(2);
		return(-1);
	}

	sock->sa.sin_family = AF_INET;
	sock->sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sock->sa.sin_port = htons(port);
	ret = bind(sock->s, (SOCKADDR*)&sock->sa, sizeof(struct sockaddr));
	if (ret == INVALID_SOCKET) {
		printf("Error binding to receive socket\n");
		sleep(2);
		return(-1);
	}
	printf("receive socket bound successfully\n");

	sscanf(addr,"%d",&first_octet);
//	printf("first octet of ip address is %d\n",first_octet);
	// if(first_octet>=224 && first_octet<=239) {
	// 	struct ip_mreq imr;
	// 	printf("\tmulticast address detected: multicast mode enabled\n");
	// 	imr.imr_multiaddr.s_addr  = inet_addr(addr);
	// 	imr.imr_interface.s_addr  = htonl(INADDR_ANY);
	// 	ret = setsockopt(sock->s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&imr, sizeof(imr));
	// 	if (ret != 0) {
	// 		printf("Error setting multicast membership on recieve socket\n");
	// 		sleep(2);
	// 		return(-1);
	// 	}
	// 	printf("\tmulticast configured successfully on receive socket\n");
	// }
	return 0;
}

//defines create transmit socket function *this one also binds to a specific source port*
int setupTransmitSocket(udp_sock_struct_t* sock, const char* addr, short int port) {

	// Create a UDP datagram socket
	sock->s = socket(	AF_INET,		// Address family
						SOCK_DGRAM,	// Socket type
						IPPROTO_UDP);	// Protocol
	if (sock->s == INVALID_SOCKET) {
		printf("Error creating command transmit socket\n");
		return(-1);
	}

	// Fill in the address structure for the server
	sock->sa.sin_family = AF_INET;				// Internet address family
	sock->sa.sin_addr.s_addr = inet_addr(addr);
	sock->sa.sin_port = htons(port);			// Port number

	return 0;
}

//defines create transmit socket function *this one also binds to a specific source port*
int setupSpecificTransmitSocket(udp_sock_struct_t* sock,  const char* addr,  short int port,
								udp_sock_struct_t* sock2, const char* addr2, short int port2) {

	// Create a UDP datagram socket
	sock->s = socket(	AF_INET,		// Address family
						SOCK_DGRAM,		// Socket type
						IPPROTO_UDP);	// Protocol
	if (sock->s == INVALID_SOCKET) {
		printf("Error creating command transmit socket\n");
		return(-1);
	}

	// Fill in the address structure for the server
	sock->sa.sin_family = AF_INET;				// Internet address family
	sock->sa.sin_addr.s_addr = inet_addr(addr);
	sock->sa.sin_port = htons(port);			// Port number

	int ret;
	sock2->s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sock2->s == INVALID_SOCKET) {
		printf("Error creating command transmit socket on host end\n");
		return(-1);
	}
	sock2->sa.sin_family = AF_INET;
	sock2->sa.sin_addr.s_addr = inet_addr(addr2);
	//sock2->sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sock2->sa.sin_port = htons(port2);
	ret = bind(sock->s, (SOCKADDR*)&sock2->sa, sizeof(struct sockaddr));
	if (ret == INVALID_SOCKET) {
		printf("Error binding transmit socket to host source port\n");
		sleep(2);
		return(-1);
	}

	return 0;
}
