// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "entry.h"
#include "tree.h"
#include "tree-private.h"
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
	memcpy(*data_buf + sizeof(int), data->data, data->datasize);

	return tamanho;
}

/* De-serializa a mensagem contida em data_buf, com tamanho
 * data_buf_size, colocando-a e retornando-a numa struct
 * data_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct data_t *buffer_to_data(char *data_buf, int data_buf_size){
	struct data_t *data;
	int datasize;
	int offset = 0;

	if(data_buf == NULL || data_buf_size == -1)
		return NULL;

	memcpy(&(datasize), data_buf, sizeof(int));                 offset += sizeof(int);
	data = data_create(datasize);
	memcpy(data->data, data_buf + offset, datasize);            offset += data->datasize;

	// if ( data_buf_size != offset ) // o comprimento recebido nao e compativel com o calculado
	// error!

	return data;
}

/* Serializa uma estrutura entry num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer ou -1 em caso de erro.
 */
int entry_to_buffer(struct entry_t *data, char **entry_buf){
	char *aux;
	int strl;
	int tamanho_key;
	int tamanho_data;
	int offset = 0;
	if(data == NULL || entry_buf == NULL)
		return -1;

	strl = strlen(data->key)+1;

	tamanho_key = sizeof(int) + strl;
	*entry_buf = (char*)malloc(tamanho_key* sizeof(char));
	memcpy(*entry_buf + offset, &strl, sizeof(int));              offset += sizeof(int);
	memcpy(*entry_buf + offset, data->key, strl);                 offset += strl;

	tamanho_data = data_to_buffer(data->value, &aux);
	*entry_buf = realloc(*entry_buf, offset + tamanho_data);
	memcpy(*entry_buf + tamanho_key, aux, tamanho_data);          offset += tamanho_data;
	free(aux);

	return offset;
}

/* De-serializa a mensagem contida em entry_buf, com tamanho
 * entry_buf_size, colocando-a e retornando-a numa struct
 * entry_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
	int offset=0;
	int key_size =-1;
	int data_size=-1;
	char* key;
	struct entry_t *entry;
	struct data_t *data;

	if(entry_buf == NULL || entry_buf_size == -1)
		return NULL;

	memcpy(&key_size, entry_buf + offset, sizeof(int));                 offset += sizeof(int);
	key = (char *)malloc(key_size);
	memcpy(key ,entry_buf + offset, key_size);                          offset += key_size;
	data_size= entry_buf_size - offset;
	data = buffer_to_data(entry_buf+ offset, data_size);                offset += data_size;
	entry = entry_create(key, data);
	return entry;
}

/*[0/1] [tamanho entry][entry][tamanho node esquerdo][node esquerdo][tamanho node direito][node direito]
 *
 */
int node_to_buffer(struct node_t *node, char **node_buf){
	int node_buf_size;
	char *aux;
	int aux_size;

	node_buf_size = 0;

	if(node_buf==NULL)
		return -1;

	// sem node preenchemos '0'
	if ( node == NULL ){
		// preencher 0
		*node_buf= (char*)malloc(sizeof(char));
		memcpy(*node_buf, "0", sizeof(char));
		node_buf_size += sizeof(char);
	}else{// com node preenchemos '1' e serializamos o entry
		// preencher 1
		*node_buf= (char*)malloc(sizeof(char));
		memcpy(*node_buf, "1", sizeof(char));                      node_buf_size += sizeof(char);

		// serializar entry
		aux_size=entry_to_buffer(node->entry, &aux);
		*node_buf= realloc(*node_buf, node_buf_size + aux_size + sizeof(int));
		memcpy(*node_buf + node_buf_size, &aux_size, sizeof(int)); node_buf_size+= sizeof(int);
		memcpy(*node_buf + node_buf_size, aux, aux_size);          node_buf_size+= aux_size;
		free(aux);
		// serializar filho esquerda
		aux_size = node_to_buffer(node->left, &aux);
		*node_buf = realloc(*node_buf, node_buf_size + aux_size+ sizeof(int));
		memcpy(*node_buf + node_buf_size, &aux_size, sizeof(int)); node_buf_size+= sizeof(int);
		memcpy(*node_buf + node_buf_size, aux, aux_size);          node_buf_size+= aux_size;
		free(aux);
		// serializar filho direita
		aux_size = node_to_buffer(node->right, &aux);
		*node_buf = realloc(*node_buf, node_buf_size + aux_size + sizeof(int));
		memcpy(*node_buf + node_buf_size, &aux_size, sizeof(int)); node_buf_size+= sizeof(int);
		memcpy(*node_buf + node_buf_size, aux, aux_size);          node_buf_size+= aux_size;
		free(aux);
	}

	return node_buf_size;
}



/* Serializa uma estrutura tree num buffer que será alocado
 * dentro da função. Além disso, retorna o tamanho deste
 * buffer ou -1 em caso de erro.
 */
int tree_to_buffer(struct tree_t *tree, char **tree_buf){
	if(tree==NULL || tree_buf==NULL){
		return -1;
	}
	return node_to_buffer(tree->raiz, tree_buf);
}


struct node_t * buffer_to_node(char *node_buf, int node_buf_size ){
	int offset = 0;
	int entry_size;
	int left_node_size;
	int right_node_size;
	struct entry_t *entry;
	struct node_t *node_right;
	struct node_t *node_left;
	struct node_t *node;


	//if ( memcmp(node_buf, "0", sizeof(char)) == 0 )
	if ( node_buf[offset] == '0' )
		node= NULL;
	else {// so pode ser 1
		offset += sizeof(char);
		memcpy(&entry_size, &node_buf[offset], sizeof(int));
		offset += sizeof(int);
		entry = buffer_to_entry(&node_buf[offset], entry_size);
		offset += entry_size;

		memcpy(&left_node_size, &node_buf[offset], sizeof(int) );
		offset += sizeof(int);
		node_left = buffer_to_node (&node_buf[offset], left_node_size);
		offset+= left_node_size;

		memcpy(&right_node_size, &node_buf[offset], sizeof(int) );
		offset += sizeof(int);
		node_right = buffer_to_node (&node_buf[offset], right_node_size);
		offset += right_node_size;

		node = node_create(entry);
		node->left = node_left;
		node->right = node_right;
	}
	return node;
}




/* De-serializa a mensagem contida em tree_buf, com tamanho
 * tree_buf_size, colocando-a e retornando-a numa struct
 * tree_t, cujo espaco em memoria deve ser reservado.
 * Devolve NULL em caso de erro.
 */
struct tree_t *buffer_to_tree(char *tree_buf, int tree_buf_size){
	if(tree_buf==NULL || tree_buf_size<=0){
		return NULL;
	}
	struct tree_t *tree;
	tree= tree_create();
	tree-> raiz= buffer_to_node(tree_buf,tree_buf_size );
	return tree;
}