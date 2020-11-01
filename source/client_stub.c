// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sdmessage.pb-c.h"
#include "message-private.h"
#include "client_stub.h"
#include "client_stub-private.h"
#include "network_client.h"
#include "data.h"
#include "entry.h"
#include "tree.h"
#include "serialization.h"
#include "inet.h"

/* Remote tree. A definir pelo grupo em client_stub-private.h
 */
//struct rtree_t;

/* Função para estabelecer uma associação entre o cliente e o servidor, 
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna NULL em caso de erro.
 */
struct rtree_t *rtree_connect(const char *address_port){
  if(address_port == NULL){
    return NULL;
  }
  //Criaçao de rtree e seus atributos
  struct rtree_t *rt = (struct rtree_t *) malloc(sizeof(struct rtree_t));
  if (rt == NULL){
    perror("Falta de memoria\n");
    return NULL;
  }
  rt->hostname =(char *) malloc(sizeof(char *)* 50);
  rt->port =(char *) malloc(sizeof(char *)*10);
  //usamos strcpy porque temos a certeza que eh uma string
  strcpy(rt->hostname, strtok(address_port,":"));
  strcpy(rt->port, strtok(NULL,":"));

  if (network_connect(rt) == -1){
    free(rt->hostname);
    free(rt->port);
    free(rt);
    return NULL;
  }
  return rt;
}

/* Termina a associação entre o cliente e o servidor, fechando a 
 * ligação com o servidor e libertando toda a memória local.
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtree_disconnect(struct rtree_t *rtree){
    if(rtree == NULL)
        return -1;
    return network_close(rtree);
}

/* Função para adicionar um elemento na árvore.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Devolve 0 (ok, em adição/substituição) ou -1 (problemas).
 */
int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
  int res = -1;
  struct message_t msg, * rmsg;
  message_t__init(&msg);

  if(rtree == NULL || entry==NULL)
    return res;

  if (put_request_message(&msg, entry) == -1)
    return res;
  
  rmsg = network_send_receive(rtree, &msg);
  //apos o envio e a recepcao das mensagens, free do
  //allocado previamente em put_request_message
  free(msg.keys);
  free(msg.data);
  //entry_destoy(entry);
  //free(entry->value->data);
  free(entry);
  if (rmsg != NULL){
    if (rmsg->opcode == 41){//existe resposta do servidor
      res = 0;
    }
    message_t__free_unpacked(rmsg, NULL);
  }
  return res;
}

/* Função para obter um elemento da árvore.
 * Em caso de erro, devolve NULL.
 */
struct data_t *rtree_get(struct rtree_t *rtree, char *key){
 struct data_t *res = NULL;
  struct message_t msg, * rmsg;
  message_t__init(&msg);
  
  if(rtree == NULL || strlen(key)<=0)
    return res;

  if (get_request_message(&msg, key) == -1)
    return res;

  rmsg = network_send_receive(rtree, &msg);
  //apos o envio e a recepcao das mensagens, free do
  //allocado previamente em put_request_message
  free(msg.data);
  if (rmsg != NULL){
		if (rmsg->opcode == 31)
    {
      if (rmsg->data_size >0)
			{//existe resposta do servidor
        res = data_create(rmsg->data_size +1);
        if (res != NULL)
        	memcpy(res->data, rmsg->data, res->datasize);
        //res->data[res->datasize] = '\0';
      }
      else
      {//size==0
				res = NULL;
			}
    }
    else
    {//opcode == 99
			res = NULL;
    }
    message_t__free_unpacked(rmsg, NULL);
  }
  return res;
}




/* Função para remover um elemento da árvore. Vai libertar 
 * toda a memoria alocada na respetiva operação rtree_put().
 * Devolve: 0 (ok), -1 (key not found ou problemas).
 */
int rtree_del(struct rtree_t *rtree, char *key){
  int res = -1;
  struct message_t msg, * rmsg;
  message_t__init(&msg);

  if(rtree == NULL || strlen(key)<=0)
    return res;

  if (del_request_message(&msg, key) == -1)
    return res;
  
  rmsg = network_send_receive(rtree, &msg);
  //apos o envio e a recepcao das mensagens, free do
  //allocado previamente em put_request_message
  free(msg.data);
  if (rmsg != NULL){
		if(rmsg->opcode == 99){
			printf("Nao existe essa key para eliminar.\n");
		}
    else if (rmsg->opcode == 21){//existe resposta do servidor
			printf("A chave foi %s eliminada.\n", key);
      res = 0;
    }
    message_t__free_unpacked(rmsg, NULL);
  }
  return res;
}




/* Devolve o número de elementos contidos na árvore.
 */
int rtree_size(struct rtree_t *rtree){
  int res = -1;
  struct message_t msg, * rmsg;
  message_t__init(&msg);

  if(rtree == NULL)
    return res;

  size_request_message(&msg);
  
  rmsg = network_send_receive(rtree, &msg);
  if (rmsg != NULL){
    if (rmsg->opcode == 11){//existe resposta do servidor
      res = rmsg->result;
    }
    message_t__free_unpacked(rmsg, NULL);
  }
  return res;
}

/* Função que devolve a altura da árvore.
 */
int rtree_height(struct rtree_t *rtree){
  int res = -1;
  struct message_t msg, * rmsg;
  message_t__init(&msg);

  if(rtree == NULL)
    return res;

  height_request_message(&msg);

  rmsg = network_send_receive(rtree, &msg);
  
  if (rmsg != NULL){
    if (rmsg->opcode == 61){//existe resposta do servidor
      res = rmsg->result;
    }
    message_t__free_unpacked(rmsg, NULL);
  }
  
  return res;
}








/* Devolve um array de char* com a cópia de todas as keys da árvore,
 * colocando um último elemento a NULL.
 */
char **rtree_get_keys(struct rtree_t *rtree){
  char ** res = NULL;
  struct message_t msg, * rmsg;
  message_t__init(&msg);
  get_keys_request_message(&msg);
  rmsg = network_send_receive(rtree, &msg);
	if (rmsg != NULL){
    if (rmsg->opcode == 51){//existe resposta do servidor
      res= malloc(rmsg->data_size);
      strcpy(res, rmsg->data);
    }
    message_t__free_unpacked(rmsg, NULL);
		
  }
	return res;

}





/* Liberta a memória alocada por rtree_get_keys().
 */
void rtree_free_keys(char **keys){
  tree_free_keys(keys);
}
