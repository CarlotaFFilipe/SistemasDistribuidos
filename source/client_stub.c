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
#include <zookeeper/zookeeper.h>



typedef struct String_vector zoo_string;

zhandle_t * czh = NULL;
FILE * fp;
const char * z_root = "/kvstore";            ////////////////////mudar o nome desta variavel
char * primary_server = NULL;
char * backup_server = NULL;
struct rtree_t * primary_rtree = NULL;
struct rtree_t * backup_rtree = NULL;


int update_servers();
char * find_server(zoo_string * children, int pos);
void free_zoostring (zoo_string * children);

//watch da ligação deste cliente
void cconnection_watcher(zhandle_t * zkh, int type, int state, const char * path, void * ctx){}




//watch dos servidores
static void cchild_watcher(zhandle_t * zkh, int type, int state, const char * path, void * ctx) {
	if (state == ZOO_CONNECTED_STATE){
		if (type == ZOO_CHILD_EVENT){
      if (update_servers() < 0){
        perror("Erro na atualizacao dos servidores.");
      }
		}
	}
}







struct rtree_t * client_connect(const char *address_port){

  struct rtree_t * dummy = (struct rtree_t *) malloc(sizeof(struct rtree_t));
  if (dummy == NULL){
    perror("Falta de memoria\n");
    return NULL;
  }

	fp = fopen ("zoo_log.txt", "w+"); ///////////////Tera este nome
	if (fp == NULL){
		perror("Erro na criacao do ficheiro de log.");
	} else{
		zoo_set_log_stream(fp);
	}

  czh = zookeeper_init(address_port, cconnection_watcher, 2000, 0, 0, 0);
	if (czh == NULL) {
		perror("Erro na ligacao ao servidor do ZooKeeper.");
		return NULL;
	}

  if (ZNONODE == zoo_exists(czh, z_root, 0, NULL)) {
    perror("\nServidor inexistente.");
    return NULL;
  }
  if (update_servers() < 0){
    return NULL;
  }
  return dummy;
}




//atualiza a ligacao primaria e o backup
int update_servers(){
  zoo_string* children_list =	(zoo_string *) malloc(sizeof(zoo_string));
  if (ZOK != zoo_wget_children(czh, z_root, cchild_watcher, NULL, children_list)) {
    perror("Erro no watch.");
    return -1;
  }
//como so existem 2 servidores nesta lista, o primary e o backup, sabemos as suas posicoes
  char * primary = find_server(children_list, 0);
  char * backup = find_server(children_list, 1);
  free_zoostring(children_list);

  if (primary == NULL || backup == NULL){
    perror("\nServidor inexistente.");
    return -1;
  }

  printf("\nServidor PRIMARIO vai mudar de %s para %s.\n", primary_server, primary);

  int len = 1024;
  char nserver_addrport[1024] = "";

  if (primary_server == NULL || strcmp(primary, primary_server) != 0){
    if (primary_rtree != NULL){
      if (rtree_disconnect(primary_rtree) != 0){
        perror("Erro no fecho da ligacao com o servidor");
        return -1;
      }
      free(primary_server);
    }
    primary_server = primary;
    zoo_get(czh, primary_server, 0, &nserver_addrport, &len, NULL);
    primary_rtree = rtree_connect(nserver_addrport);
    if (primary_rtree == NULL){
      return -1;
    }
  } else {
    free(primary);
  }

	printf("\nServidor BACKUP vai mudar de %s para %s.\n", backup_server, backup);

  if (backup_server == NULL || strcmp(backup_server, backup) != 0){
    if (backup_rtree != NULL){
      if (rtree_disconnect(backup_rtree) != 0){
        perror("Erro no fecho da ligacao com o servidor");
        return -1;
      }
      free(backup_server);
    }
    backup_server = backup;
    zoo_get(czh, backup_server, 0, &nserver_addrport, &len, NULL);
    backup_rtree = rtree_connect(nserver_addrport);
    if (backup_rtree == NULL){
      return -1;
    }
  } else {
    free(backup);
  }
  return 0;
}


char * find_server(zoo_string * children, int pos){
    char * temp = NULL;
	char tnode[32] = "";
	strcat(tnode, z_root);
	strcat(tnode, "/");

	return strdup(strcat(tnode, children->data[pos]));

}

//////////////////////////////////////////////////////////////////////////////
/////////////////Nao sei se eh preciso esta funcao
//liberta memoria alocada por get_children
void free_zoostring (zoo_string * children){
	if (children != NULL){
		if(children->count > 0){
			for (int i = 0; i < children->count; i++){
				free(children->data[i]);
			}
		}
		free(children->data);
		free(children);
	}
}

int client_disconnect(struct rtree_t * dummy){
  if (czh != NULL)
  	zookeeper_close(czh);
  if (fp != NULL)
		fclose(fp);
  free(dummy);
  if (primary_rtree != NULL && rtree_disconnect(primary_rtree) != 0){
    perror("Erro no fecho da ligacao com o servidor");
    return -1;
  }
  if (backup_rtree != NULL && rtree_disconnect(backup_rtree) != 0){
    perror("Erro no fecho da ligacao com o servidor");
    return -1;
  }
  if (primary_server != NULL)
    free(primary_server);
  if (backup_server != NULL)
    free(backup_server);
  return 0;
}
























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
	free(msg.key);
	free(msg.data);
	free(entry);
	if (rmsg != NULL){
		if(rmsg->opcode == 99){
			printf("Nao foi possivel inserir essa entry.\n");
		}
		else if (rmsg->opcode == 41){//existe resposta do servidor
			res = rmsg->res;
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
		if (rmsg->opcode == 31){
			if (rmsg->data_size >0){//existe resposta do servidor
				res = data_create(rmsg->data_size +1);
				if (res != NULL)
					memcpy(res->data, rmsg->data, res->datasize);
				//res->data[res->datasize] = '\0';
			}else{//size==0
				res = NULL;
			}
		}
		else{//opcode == 99
			printf("Nao existe essa key para eliminar.\n");
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
			res = rmsg->res;;
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
		if(rmsg->opcode == 99){
			printf("Nao foi possivel calcular o tamanho da tree.\n");
		}
		else if (rmsg->opcode == 11){//existe resposta do servidor
			res = rmsg->res;
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
		if(rmsg->opcode == 99){
			printf("Nao foi possivel calcular a altura da tree.\n");
		}
		else if (rmsg->opcode == 61){//existe resposta do servidor
			res = rmsg->res;
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
		if(rmsg->opcode == 99){
			printf("Nao efoi possivel obter todas as chaves da arvore.\n");
		}
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

/* Verifica se a operação identificada por op_nfoi executada.*/
int rtree_verify(struct rtree_t *rtree, int op_n){
	int res = -1;
	struct message_t msg, * rmsg;
	message_t__init(&msg);

	verify_request_message(&msg);

	rmsg = network_send_receive(rtree, &msg);

	if(rtree == NULL){
		return res;
	}
	if (rmsg != NULL){
		if(rmsg->opcode == 99){
			printf("Nao foi possivel verificar se a operacao %d ja foi executada.\n", op_n);
		}
		else if (rmsg->opcode == 71){  //existe resposta do servidor
			res = rmsg->res;
		}
		message_t__free_unpacked(rmsg, NULL);
	}
	return res;
}