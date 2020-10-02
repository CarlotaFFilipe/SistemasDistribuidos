// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados passados.
 */
struct entry_t *entry_create(char *key, struct data_t *data){
  if(key == NULL || data == NULL || data->datasize <= 0 || data->data == NULL) 
    return NULL;
  struct entry_t *entry = malloc(strlen(key) * data->datasize * sizeof(struct entry_t));
  entry->key = key; //memcpy
  entry->value = data; //memcpy
  return entry;
}

/* Função que inicializa os elementos de uma entrada na tabela com o
 * valor NULL.
 */
void entry_initialize(struct entry_t *entry){
  if(entry != NULL){
    entry->key = NULL;
    //Nao sei qual usar
    entry->value = data_create(0);
    //entry->value = NULL;
  }
}

/* Função que elimina uma entry, libertando a memória por ela ocupada
 */
void entry_destroy(struct entry_t *entry){
  if(entry != NULL){
    data_destroy(entry->value);
    free(entry->key);
    entry->key = NULL; //Pode nao ser necessario
    free(entry);
    entry = NULL; //Pode nao ser necessario
  }
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 */
struct entry_t *entry_dup(struct entry_t *entry){
  if(entry == NULL)//entry->value->datasize <= 0 || entry->value->data == NULL
    return NULL;
  else{
    struct entry_t *entrydup = malloc(sizeof(struct entry_t));
    char *keydup = malloc(strlen(entry->key) * sizeof(char) + 1); //se bem me lembro,em c sabemos que eh o final de uma string quando encontramos \0.
    memcpy(keydup, entry->key, strlen(entry->key) + 1);
    entrydup->key = keydup; //memcpy
    struct data_t *data = data_dup(entry->value);
    entrydup->value = data; //memcpy
    return entrydup;
  }
}

/* Função que substitui o conteúdo de uma entrada entry_t.
*  Deve assegurar que destroi o conteúdo antigo da mesma.
*/
void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
  if(entry == NULL || strlen(new_key) <=0 || new_key==NULL || new_value == NULL)
    return;
  //free(entry->value);
//entry->value=new_value;
  data_replace(entry->value, new_value->datasize, new_value);
  entry->value=new_value;
  free(entry->key);
  entry->key= new_key;

}

/* Função que compara duas entradas e retorna a ordem das mesmas.
*  Ordem das entradas é definida pela ordem das suas chaves.
*  A função devolve 0 se forem iguais, -1 se entry1<entry2, e 1 caso contrário.
*/
int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
  if(entry1==NULL || entry2 == NULL)
    return;
  int comp = strcmp(entry1->key, entry2->key);
  if(comp == 0)
    return 0;
  else if(comp < 0)
    return -1;
  else
    return 1;
}
