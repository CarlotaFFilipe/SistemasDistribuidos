// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size
 */
struct data_t *data_create(int size){
  if(size <= 0)
    return NULL;
  struct data_t *d1= (struct data_t*) malloc(size * sizeof(struct data_t));
  d1->datasize = size;
  d1->data = malloc(size * sizeof(d1->data));
  return d1;
}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void *data){
  if(data == NULL || size <= 0)
    return NULL;
  struct data_t *d2 = (struct data_t*) malloc(size * sizeof(struct data_t));
  d2->datasize = size;
  d2->data = data; //deveriamos fazer memcpy?
  return d2;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro data,
 * libertando toda a memória por ele ocupada.
 */
void data_destroy(struct data_t *data){
  if(data != NULL){
    free(data->data);
    //data->data = NULL;
    free(data);
    //data = NULL;
  }
}

/* Função que duplica uma estrutura data_t, reservando a memória
 * necessária para a nova estrutura.
 */
struct data_t *data_dup(struct data_t *data){
  if(data == NULL || data->datasize <= 0 || data->data == NULL)
    return NULL;
  else{
    struct data_t *dup = (struct data_t*) malloc(sizeof(struct data_t));
    dup->datasize = data->datasize;
    dup->data = malloc(data->datasize * sizeof(char));
    memcpy(dup->data, data->data, data->datasize);
    return dup;
  }
}

/* Função que substitui o conteúdo de um elemento de dados data_t.
*  Deve assegurar que destroi o conteúdo antigo do mesmo.
*/
void data_replace(struct data_t *data, int new_size, void *new_data){
  if(data == NULL || data->datasize <= 0 || data->data == NULL || new_size <= 0 || new_data == NULL)
    return;
  free(data->data);
  data->data = NULL;
  data->datasize = new_size;
  data->data = new_data;
}