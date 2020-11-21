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

  q->head = NULL;
  q->tail = NULL;
  q->size = 0;

  return q;
}

int enqueue(struct queue_t * q, struct task_t * task){
  if (q == NULL || task == NULL){
    return -1;
  }
  if (q->tail == NULL) {
    q->head = task;
    q->tail = task;
  } else {
    q->tail->next = task;
    q->tail = task;
  }
  q->size ++;
  return 0;
}


struct task_t * dequeue(struct queue_t * q){
  if (q->head == NULL){
    return NULL;
  }

  struct task_t * temp = q->head;

  q->head = q->head->next;
  if (q->head == NULL){
    q->tail = NULL;
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
  while(q->head != NULL){
    destroy_task(dequeue(q));
  }
  free(q);
}

struct task_t *empty_queue(struct queue_t *queue, pthread_mutex_t queue_lock, pthread_cond_t queue_not_empty){
	pthread_mutex_lock(&queue_lock);
	int size = queue_size(queue);
	if (size == 0)//ficar a espera caso nao haja nenhuma thread na fila
		pthread_cond_wait(&queue_not_empty, &queue_lock);
	//como existe tasks, consumir uma task da fila
	struct task_t *task = dequeue(queue);
	pthread_mutex_unlock(&queue_lock);

	return task;
}