// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <pthread.h>
#include <unistd.h>
//struct que guarda a cabeca e a cauda da fila assim como o seu tamanho
struct queue_t {
  struct task_t * head;
  struct task_t * tail;
  int size;
};

//cria uma fila
struct queue_t * create_queue();

//adiciona a task na fila q
int enqueue(struct queue_t * q, struct task_t * task);

//remove a cabeca da fila q e devolve-a. a memoria alocada pela task nao e libertada
struct task_t * dequeue(struct queue_t * q);

//o tamanho da fila q
int queue_size(struct queue_t * q);

//destroi a fila q e liberta todo o espaco por ela ocupado
void queue_destroy(struct queue_t * q);

struct task_t *empty_queue(struct queue_t *queue, pthread_mutex_t queue_lock, pthread_cond_t queue_not_empty);