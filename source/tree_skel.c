// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


#include "sdmessage.pb-c.h"
#include "tree_skel.h"
#include "message-private.h"
#include "entry.h"
#include "data.h"
#include "tree.h"
#include "serialization.h"
#include "task.h"
#include "queue.h"
#include "client_stub.h"
#include "client_stub-private.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


int verify(int op_n);
struct tree_t *tree;
struct queue_t *queue;

bool stop_flag = true;
int last_assigned = 0, op_count = 0;
pthread_t thread;

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER, tree_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty =  PTHREAD_COND_INITIALIZER;

int process_delete(struct task_t * task);
int process_put(struct task_t * task);


///////////////////


//ip:porto do servidor do zookeeper
const char * host_port;
//porto tcp
int tcp_port;

//Livro:Apache Zookeeper Essentials
typedef struct String_vector zoo_string;

zhandle_t * zh = NULL;
FILE * server_fp;
const char * z_root = "/kvstore";
char * node_id = NULL;
char * backup_server = NULL;
struct rtree_t * rtree = NULL;



//funcoes locais
void * process_task(void * params);
int zk_connect();
int connect2server();


char * get_ip() {

  char hostbuffer[256];
  if (gethostname(hostbuffer, sizeof(hostbuffer)) < 0){ //unistd
      perror("gethostname");
      return NULL;
  }
  struct hostent * host_entry = gethostbyname(hostbuffer);
  if (host_entry == NULL){
      perror("gethostbyname");
      return NULL;
  }
  char * IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
  if (IPbuffer == NULL){
      perror("inet_ntoa");
      return NULL;
  }
  return strdup(IPbuffer);
}



//watch da ligação deste servidor
void connection_watcher(zhandle_t * zkh, int type, int state, const char * path, void * ctx){}
//watch dos servidores
static void child_watcher(zhandle_t * zkh, int type, int state, const char * path, void * ctx) {
	if (state == ZOO_CONNECTED_STATE){
		if (type == ZOO_CHILD_EVENT){
      		connect2server();
		}
	}
}



//faz a ligacao ao servidor do zookeeper e cria o no efemero deste servidor
int zk_connect(){

	server_fp = fopen ("zoo_server_log.txt", "w+");
	if (server_fp == NULL){
		perror("Erro na criacao do ficheiro de log");
	} else {
		zoo_set_log_stream(server_fp);
	}

  zh = zookeeper_init(host_port, connection_watcher, 2000, 0, 0, 0);
	if (zh == NULL) {
		perror("Erro na ligacao ao servidor do ZooKeeper.");
		return -1;
	}

  if (ZNONODE == zoo_exists(zh, z_root, 0, NULL)) {
    if (ZOK != zoo_create(zh, z_root, NULL, -1, & ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0)) {
      perror("Erro na criacao do no /kvstore.");
      return -1;
    }
  }

  char node_path[64] = "/kvstore/primary";
  int new_path_len = 1024;
  node_id = malloc (new_path_len);////////////////tiramos????????????
  char * local_ip = get_ip();
  if (local_ip == NULL){
    return -1;
  }
  /*connseguimos fazer de forma dif
  */
  char server_addr_port[32] = "";
  strcat(server_addr_port, local_ip);
  free(local_ip);
  strcat(server_addr_port, ":");
  //char str[16];
  //sprintf(str, "%d", tcp_port);
  strcat(server_addr_port, itoa(tcp_port));

  if (ZOK != zoo_create(zh, node_path, server_addr_port, strlen(server_addr_port), & ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL | ZOO_SEQUENCE, node_id, new_path_len)) {
    perror("Erro na criacao do no efemero");
    return -1;
  }
  printf("\nNo efemero %s criado.\n", node_id);
  return 0;
}



//faz watch aos servidores, preenche struct next_server e liga-se a esse servidor
int connect2server(){

	
	int len = 1024;
    char nserver_addrport[1024] = "";
	bool has_backup = false;
	char * backup = NULL;

//Supostamente tem tamanho 2 (primary e backup)
  zoo_string* children_list =	(zoo_string *) malloc(sizeof(zoo_string));
  if (ZOK != zoo_wget_children(zh, z_root, child_watcher, NULL, children_list)) {
    perror("Erro no watch.");
    return -1;
  }
  if(children_list->count == 2)
	has_backup = true;

  if(has_backup)
  	backup = find_server(children_list, 1);
  if(backup == NULL){
    perror("Erro a encontrar servidor");
    return -1;
  }
  free_zoostring(children_list);

	//caso seja o mesmo servidor
	if (backup_server != NULL && strcmp(backup, backup_server) == 0){
		free(backup);
		return 0;
	}
  //caso so haja 1 no, so existe o primary...
  if (strcmp(backup, node_id) == 0){
		free(backup);
    if (backup_server != NULL){
      free(backup_server);
      backup_server = NULL;
      if (rtree_disconnect(rtree) != 0){
        perror("Erro no fecho da ligacao com o servidor");
        return -1;
      }
      rtree = NULL;
    }
  } else {
    if (backup_server != NULL){
      free(backup_server);
      if (rtree_disconnect(rtree) != 0){
        perror("Erro no fecho da ligacao com o servidor");
        return -1;
      }
    }
    backup_server = backup;
    zoo_get(zh, backup_server, 0, &nserver_addrport, &len, NULL);
    rtree = rtree_connect(nserver_addrport);
    if (rtree == NULL){
      return -1;
    }
  }
  return 0;
}












int put_process(struct task_t * task){
	struct data_t * value = data_create(task->datasize);
	if (value == NULL){
		return -1;
	}  
	value->data= strdup(task->data);
	char *key= strdup(task->key);

	pthread_mutex_lock(&tree_lock);
	int res = tree_put(tree, key, value);
	pthread_mutex_unlock(&tree_lock);

	data_destroy(value);
	free(key);
	return res;
}

int delete_process(struct task_t * task){
	char *key= strdup(task->key);

	pthread_mutex_lock(&tree_lock);
	int res = tree_del(tree, key);
	pthread_mutex_unlock(&tree_lock);

	free(key);
	return res;
}


void * process_task(void * params){

	struct task_t * task;
	int res = 0;
	int size = -1;

	while(stop_flag){

		pthread_mutex_lock(&queue_lock);
		size = queue_size(queue);
		if (size == 0)//ficar a espera caso nao haja nenhuma thread na fila
			pthread_cond_wait(&queue_not_empty, &queue_lock);
		//como existe tasks, consumir uma task da fila
		task = dequeue(queue);
		pthread_mutex_unlock(&queue_lock);
		if (task == NULL){
			continue;
		}
		if (task->op == 0){
			res = delete_process(task);
		} else {
			res = put_process(task);
		}
		if (res < 0){
			printf("\nOcorreu um erro na operacao %d\n\n", task->op_n);
		}
		printf("\nOperacao %d foi executada\n\n", task->op_n);
		destroy_task(task);
		op_count ++;
	}
	return NULL;
}

///////////////////////////////////////////////////////////////




/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(){
	int res= 0;
	tree = tree_create();
	queue = create_queue();
	if (tree == NULL || queue == NULL){
		res = -1;
	}else if (pthread_create(&id, NULL, process_task, NULL) != 0){
		queue_destroy(queue);
	    table_destroy(tree);
        perror("Thread não criada.");
        res = -1;
	}
	if (zk_connect() < 0){
		table_skel_destroy();
    	perror("Erro na ligacao ao zookeeper");
    	return -1;
  	}
  	if (connect2server() < 0){
		table_skel_destroy();
    	perror("Erro na ligacao ao proximo servidor");
    	return -1;
 	}
	return res;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){	
	stop_flag = false;
	if (zh != NULL)
  		zookeeper_close(zh);
	fclose(server_fp);
	if (node_id != NULL)
  		free(node_id);
	if (backup_server != NULL)//?????
  		free(backup_server);

	pthread_cond_signal(&queue_not_empty);
	if (pthread_join(thread, NULL) != 0){
		perror("Join.");
	}
	queue_destroy(queue);
	if(tree!=NULL)
		tree_destroy(tree);
}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
 */
int invoke(struct message_t *msg){
	if(tree == NULL || msg == NULL){
		return -1;
	}

	if(msg->opcode == 10){
		if(msg->c_type!=60){
			printf("A mensagem size foi composta incorretamente\n");
			return -1;
		}
		pthread_mutex_lock(&tree_lock);
		int size = tree_size(tree);
		pthread_mutex_unlock(&tree_lock);
		size_response_message(msg, size);
		return 0;

	} else if(msg->opcode == 20){ 
		if(msg->c_type!=10 || msg->n_keys != 1){
			printf("A mensagem del foi composta incorretamente\n");
			return -1;
		}

		struct task_t * task = create_task(last_assigned, 0, strdup(msg->data), NULL, 0);
		free(msg->data);
		if(task == NULL){
			error_response_message(msg);
		} else{
			del_response_message(msg, last_assigned);
			last_assigned ++;
		}
		pthread_mutex_lock(&queue_lock);
		enqueue(queue, task);
		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_lock);
		return 0;

	} else if(msg->opcode == 30){ 
		if(msg->c_type!=10 || msg->n_keys != 1){
			printf("A mensagem get foi composta incorretamente\n");
			return -1;
		}

		pthread_mutex_lock(&tree_lock);
		struct data_t *data=tree_get(tree, msg->data);
		pthread_mutex_unlock(&tree_lock);

		free(msg->data);
		if(data ==NULL){
			key_not_found_response_message(msg);
		} else{
			if(get_response_message(msg, data)==-1)
				return -1;
		}
		data_destroy(data);
		return 0;

	} else if(msg->opcode == 40){
		if(msg->c_type!=30 || msg->n_keys != 1){
			printf("A mensagem put foi composta incorretamente\n");
			return -1;
		}
		struct task_t * task = create_task(last_assigned, 1, strdup(msg->key), strdup(msg->data), msg->data_size);
		free(msg->data);
		free(msg->key);
		msg->key = NULL;
		msg->data = NULL;

		pthread_mutex_lock(&queue_lock);
		enqueue(queue, task);
		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_lock);

		if (task == NULL){
			error_response_message(msg);
		}else{
			put_response_message(msg, last_assigned);
			last_assigned ++;
		}
		return 0;

	} else if(msg->opcode == 50){ //getkeys
		//A mensagem foi composta incorretamente
		if(msg->c_type!=60){
			printf("A mensagem getkeys foi composta incorretamente\n");
			return -1;
		}
		pthread_mutex_lock(&tree_lock);
		int size = tree_size(tree);
		char** keys = tree_get_keys(tree);
		pthread_mutex_unlock(&tree_lock);
		if(keys == NULL){
			return -1;
		}
		get_keys_response_message(msg, keys, size);
		tree_free_keys(keys);
		return 0;

	} else if(msg->opcode == 60){//height
		//A mensagem foi composta incorretamente
		if(msg->c_type!=60){
			printf("A mensagem height foi composta incorretamente\n");
			return -1;
		}

		pthread_mutex_lock(&tree_lock);
		int tamanho = tree_height(tree);
		pthread_mutex_unlock(&tree_lock);

		height_response_message(msg, tamanho);
		return 0;

	}else if(msg->opcode==70){
		if(msg->c_type!=50){
			printf("A mensagem verify foi composta incorretamente\n");
			return -1;
		}
		int op_verified = verify(msg->res);
		if(op_verified == -1){
			error_response_message(msg);
		}else{
			verify_response_message(msg, op_verified);
		}
		return 0;


	} else{//opcode 99, o servidor nao deve receber este opcode
		error_response_message(msg);
		return -1;
	}

}

/* Verifica se a operação identificada por op_n foi executada.
 * return 1 se a operacao ja foi executada
 * return 0 caso ainda esteja na queue, nao exista ou esta correntemente a ser executada
 */
int verify(int op_n){
	return op_n < op_count;
}