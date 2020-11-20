// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

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

//o tamanho da fila q
int queue_size(struct queue_t * q);

//destroi a fila q e liberta todo o espaco por ela ocupado
void queue_destroy(struct queue_t * q);