// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


//TODO: ver as respostas do get e del

#include "sdmessage.pb-c.h"
#include "tree_skel.h"
#include "message-private.h"
#include "entry.h"
#include "data.h"
#include "tree.h"
#include "serialization.h"
#include "task.h"
#include "queue.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


int verify(int op_n);
struct tree_t *tree;
struct queue_t *queue;
int ts_stop_flag = 0;

int last_assigned = 0, op_count = 0;
pthread_t thread;

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER, tree_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty =  PTHREAD_COND_INITIALIZER;

int process_delete(struct task_t * task);
int process_put(struct task_t * task);

///////////////////////////FASE 3

int process_put(struct task_t * task){
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

int process_delete(struct task_t * task){
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

	while(ts_stop_flag == 0){
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
			res = process_delete(task);
		} else {
			res = process_put(task);
		}
		if (res < 0){
			printf("\nErro na operacao %d\n\n", task->op_n);
		}
		printf("\nOperacao %d concluida\n\n", task->op_n);
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
	}else{
		if (pthread_create(&thread, NULL, process_task, NULL) != 0){
			perror("Thread não criada.");
			return -1;
		}
	}
	return res;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){
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
			msg->opcode = 99;
		} else{
			msg->opcode +=1;
			last_assigned ++;
		}
		del_response_message(msg);
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
		if (task == NULL){
			msg->opcode = 99;
		}else{
			msg->opcode += 1;
			last_assigned ++;
		}
		put_response_message(msg);

		pthread_mutex_lock(&queue_lock);
		enqueue(queue, task);
		pthread_cond_signal(&queue_not_empty);
		pthread_mutex_unlock(&queue_lock);
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

		////////////////////////////////////sera preciso fase lock e unlock???????????????
	}else if(msg->opcode==70){
		if(msg->c_type!=50){
			printf("A mensagem verify foi composta incorretamente\n");
			return -1;
		}
		int ver = verify(msg->res);
		if(ver == -1){
			error_response_message(msg);
		}else{
			verify_response_message(msg, ver);
		}
		return 0;


	} else{//opcode 99, o servidor nao deve receber este opcode
		error_response_message(msg);
		return -1;
	}

}

/* Verifica se a operação identificada por op_n foi executada.*/
int verify(int op_n){
	return op_n < op_count;
}