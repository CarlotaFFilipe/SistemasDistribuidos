


#include <stdio.h>
#include <stdlib.h>
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