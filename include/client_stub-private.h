// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H
//#include "inet.h"
#include "client_stub.h"

struct rtree_t{
	//struct sockaddr_in server;
	char *hostname;
	char *port;
    int socket;
};
#endif