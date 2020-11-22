// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include "task.h"




struct queue_t * create_queue(){

	struct queue_t * q = (struct queue_t *)malloc(sizeof(struct queue_t));
	if (q == NULL){
		return NULL;
	}

	q->cabeca = NULL;
	q->cauda = NULL;
	q->size = 0;

	return q;
}

int enqueue(struct queue_t * q, struct task_t * task){
	if (q == NULL || task == NULL){
		return -1;
	}//queue com apenas 1 apos a insercao
	if (q->cauda == NULL) {
		q->cabeca = task;
		q->cauda = task;
	} else {
		q->cauda->next = task;
		q->cauda = task;
	}
	q->size ++;
	return 0;
}


struct task_t * dequeue(struct queue_t * q){
	if (q->cabeca == NULL){
		return NULL;
	}
	//sera que devia libertar a memoria aqui????
	struct task_t * temp = q->cabeca;
	q->cabeca = q->cabeca->next;//atualiza o apontador para a "nova" cabeca

	if (q->cabeca == NULL){//queue sa tinha 1 elemento, por isso fica vazia
		q->cauda = NULL;
	}

	q->size --;
	return temp;

}

int queue_size(struct queue_t * q){
	return q->size;
}

void queue_destroy(struct queue_t * q){
	if (q == NULL)
		return;
	while(q->cabeca != NULL){
		destroy_task(dequeue(q));
	}
	free(q);
}