// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "sdmessage.pb-c.h"
#include "data.h"
#include "entry.h"
#include "serialization.h"

//Renomeia a estrutura criada pelo protobuf
#define message_t _MessageT


int snd_msg_socket (struct message_t * msg, int socket);

struct message_t * rcv_msg_socket (int socket);

int read_nbytes (int socket, void * buf, int length);

int write_nbytes (int socket, void * buf, int length);

int put_request_message(struct message_t * msg, struct entry_t * entry);

int put_response_message(struct message_t * msg);

int get_request_message(struct message_t * msg, char * key);

int get_response_message(struct message_t * msg, struct data_t * res);

int del_request_message(struct message_t * msg, char * key);

int del_request_message(struct message_t * msg, char * key);

void size_request_message(struct message_t * msg);

void size_response_message(struct message_t * msg, int size);

void height_request_message(struct message_t * msg);

void height_response_message(struct message_t * msg, int height);

void get_keys_request_message(struct message_t * msg);

int get_keys_response_message(struct message_t * msg, char ** keys, int n_keys);

void error_response_message(struct message_t * msg);


#endif