// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "task.h"

struct task_t* create_task(int op_n, int op, char* key, char* data, int datasize){

  struct task_t * task = (struct task_t*)malloc(sizeof(struct task_t));
  if (task == NULL){
    return NULL;
  }

  task->key = key;
  task->data = data;
  task->datasize = datasize;
  task->op_n = op_n;
  task->op = op;
  task->next = NULL;

  return task;
}

void destroy_task(struct task_t * task){
  if (task != NULL){
    if (task->key != NULL)
      free(task->key);
    if (task->data != NULL)
      free(task->data);
    free(task);
  }
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
