// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "entry.h"
#include "serialization.h"

/* Serializa uma estrutura data num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho do buffer
 * alocado ou -1 em caso de erro.
 */
int data_to_buffer(struct data_t *data, char **data_buf){
  if(data == NULL || data_buf == NULL){
    return -1;
  }

  int tamanho = (sizeof(int) + data->datasize);
  *data_buf = malloc(tamanho);

  memcpy(*data_buf, &(data->datasize), sizeof(int));
  memcpy(*data_buf + sizeof(int),data->data,data->datasize);

  return tamanho;
}

/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){
  if(data_buf == NULL || data_buf_size == -1)
    return NULL;
  struct data_t *data = malloc(sizeof(struct data_t));

  memcpy(&(data->datasize),data_buf,sizeof(int));
  data->data = malloc(data_buf_size-sizeof(int));
  memcpy(data->data,data_buf + sizeof(int),data_buf_size-sizeof(int));

  return data;
}

/* Serializa uma estrutura entry num buffer que sera alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer alocado ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *data, char **entry_buf){
  if(data == NULL || entry_buf == NULL)
    return -1;
  int strl = strlen(data->key)+1;
  int tamanho = (sizeof(int) + strl + sizeof(int) + data->value->datasize);

  *entry_buf = malloc(tamanho);

  memcpy(*entry_buf,&strl ,sizeof(int));
  memcpy(*entry_buf + sizeof(int), data->key,strl);
  memcpy(*entry_buf + sizeof(int) + strl,&(data->value->datasize),sizeof(int));
  memcpy(*entry_buf + sizeof(int) + strl + sizeof(int), data->value->data,data->value->datasize);

  return tamanho;
}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
  if(entry_buf == NULL || entry_buf_size == -1)
    return NULL;
  struct entry_t *entry = malloc(sizeof(struct entry_t));
  int strl =-1;
  memcpy(&strl,entry_buf,sizeof(int));
  entry->key = malloc(strl);
  memcpy(entry->key ,entry_buf + sizeof(int),strl);

  struct data_t *data = malloc(sizeof(struct data_t));

  memcpy(&(data->datasize),entry_buf +sizeof(int) +strl,sizeof(int));
  data->data = malloc(data->datasize);
  memcpy(data->data,entry_buf +sizeof(int) +strl + sizeof(int),data->datasize);

  entry->value =data;
  return entry;
}


/* Serializa uma estrutura tree num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer ou -1 em caso de erro.
 */
int tree_to_buffer(struct tree_t *tree, char **tree_buf){

}

/* De-serializa a mensagem contida em tree_buf, com tamanho
 * tree_buf_size, colocando-a e retornando-a numa struct
 * tree_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_tree(char *tree_buf, int tree_buf_size){
  
}