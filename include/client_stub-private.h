// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H
#include "inet.h"
#include "client_stub.h"

struct rtree_t{
	struct sockaddr_in server;
	char *hostname;
	char *port;
    int socket;
};



//Liga o cliente ao servidor do zookeeper e a head e tail da chain
struct rtable_t * client_connect(const char *address_port);

//Termina todas as ligacoes do cliente
int client_disconnect(struct rtable_t * rtable);

#endif