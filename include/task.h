// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


/* struct que guarda a informacao necessaria para executar um pedido de escrita
 * e um apontador para a proxima tarefa a executar
 */
struct task_t {
  int op_n;
  int op; //op = 0 delete, op = 1 put
  char* key;
  int datasize;
  char* data; //NULL em delete
  struct task_t * next;
};

//cria uma task_t com os parametros passados
struct task_t * create_task(int op_n, int op, char* key, char* data, int datasize);

//destroi a task e liberta todo o espaco em memoria por ela ocupado
void destroy_task(struct task_t * task);


//remove a cabeca da fila q e devolve-a. a memoria alocada pela task nao e libertada
struct task_t * dequeue(struct queue_t * q);
