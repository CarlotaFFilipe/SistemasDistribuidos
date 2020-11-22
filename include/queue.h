// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <pthread.h>
#include <unistd.h>
//struct que guarda a cabeca e a cauda da fila assim como o seu tamanho
struct queue_t {
  struct task_t * cabeca;
  struct task_t * cauda;
  int size;
};

//cria uma queue inicializando cada elemento a NULL ou 0
struct queue_t * create_queue();

//adiciona a task na queue q
int enqueue(struct queue_t * q, struct task_t * task);

//remove a cabeca da fila q, devolvendo-a.
struct task_t * dequeue(struct queue_t * q);

//o tamanho da queue
int queue_size(struct queue_t * q);

//destroi a queue q e liberta todo o espaco por ela ocupado
void queue_destroy(struct queue_t * q);
