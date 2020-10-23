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


/////////////////////////////////////////////////////////////
/// private functions
/////////////////////////////////////////////////////////////

struct node_t * node_create(struct entry_t *entry){
  struct node_t *newnode = (struct node_t*) malloc(sizeof(struct node_t));
  newnode->entry = entry;
  newnode->left= newnode->right= NULL;
  return newnode;
} 

void node_destroy( struct node_t *raiz ){
    if( raiz != NULL ){
      node_destroy(raiz->left);
      entry_destroy(raiz->entry);
      node_destroy(raiz->right);
      free(raiz);
    }
}


/* Função para adicionar um par chave-valor à árvore recursivamente.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.*/
struct node_t * node_insert(struct node_t* raiz, struct entry_t * entry, int *pwas_inserted){
  if(raiz == NULL){
    *pwas_inserted = 0;
    raiz = node_create(entry);
    return raiz;
  }
  int cmp= entry_compare(raiz->entry, entry);
  if(cmp == 0){
    *pwas_inserted = 0;
//destroir data antigo e alocar memoria com o novo size
    entry_destroy(raiz->entry);
    raiz->entry = entry;

  }else if(cmp == 1){
    raiz->left = node_insert(raiz->left, entry, pwas_inserted);
  }else{//if cmp == -1
    raiz->right = node_insert(raiz->right, entry, pwas_inserted);
  }
  return raiz;
}


/* Função para remover um elemento da árvore, recursivamente, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
struct node_t * node_delete(struct node_t *raiz, char *key, int *was_deleted){
  if (raiz == NULL) 
    return raiz;

  int comp = strcmp(raiz->entry->key, key);
  if (comp > 0)
    raiz->left = node_delete(raiz->left, key, was_deleted);
  else if (comp < 0)
    raiz->right = node_delete(raiz->right, key, was_deleted);
  else{
    *was_deleted = 0;

    //o node a apagar tem 2 filhos
    if(raiz->left != NULL && raiz->right != NULL){
//vai a procura do no com o valor mais baixo do ramo da direita
      struct node_t *corrente = node_min_value(raiz->right);
      entry_destroy(raiz->entry);
      // reutilizamos o mesmo node, apagamos o node original antigo mais a baixo
      raiz->entry = entry_dup(corrente->entry);
      raiz->right = node_delete(raiz->right, corrente->entry->key, was_deleted);
    }
    // o node nao tem o filho da esquerda
    else if (raiz->left == NULL){
      struct node_t *corrente = raiz->right;
      entry_destroy(raiz->entry);
      free(raiz);
      return corrente;
    }
    // o node nao tem o filho da direita
    else{ // if (raiz->right == NULL)
      struct node_t *corrente = raiz->left;
      entry_destroy(raiz->entry);
      free(raiz);
      return corrente;
    }
  }
   return raiz;
}


/* Função para obter da árvore o valor associado à chave key recursivamente.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct node_t* node_search(struct node_t *raiz, char* key){
   if(raiz == NULL)
     return raiz;
   int comp = strcmp(raiz->entry->key, key);
   if(comp==0){
     return raiz;
   }else if(comp <0){
     return node_search(raiz->right, key);
   }else{
     return node_search(raiz->left, key); 
   }
}

/*Funcao que devolve o node com o valor mais baixo
 *
 */
struct node_t * node_min_value(struct node_t *node){
   struct node_t *current = node;
   while (current && current->left != NULL)
      current = current->left;
   return current;
}

/* Função que devolve o número de elementos contidos na árvore recursivamente.
 */
int node_size(struct node_t *raiz){
  if(raiz==NULL)
    return 0;
  else 
    return (node_size(raiz->left) + 1 + node_size(raiz->right));

}

void printTree(struct tree_t* tree){
printf("\n \n");
   printT(tree->raiz);
printf("\n \n");

}

void printT(struct node_t* root){
 if(root!=NULL){
   printT(root->left);
   printf("|key: %s data: %s |",root->entry->key, (char*)root->entry->value->data);
   printT(root->right);
 }
}


/* Função que devolve a altura da árvore recursivamente.
 */
int recursive_height(struct node_t *raiz){
   if(raiz==NULL)
        return 0;
    /*else if(raiz->left == NULL && raiz->right == NULL)
        return 0;*/
    else{
        int altura_esquerda = recursive_height(raiz->left);
        int altura_direita = recursive_height(raiz->right);
        if(altura_esquerda > altura_direita)
            return altura_esquerda +1;
        else
            return altura_direita + 1;
    }
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **tree_get_keys_rec(struct node_t *raiz, char** keys, int *offset){

  if(raiz!=NULL){
    tree_get_keys_rec(raiz->left, keys, offset);
    keys[*offset]= (char*)malloc((strlen(raiz->entry->key)+1) * sizeof(char));
    strcpy(keys[*offset],raiz->entry->key );
    *offset += 1;
    tree_get_keys_rec(raiz->right, keys, offset);
  }
  return keys;
}

/////////////////////////////////////////////////////////////
/// public functions
/////////////////////////////////////////////////////////////

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){
  struct tree_t *tree = (struct tree_t*) malloc(sizeof(struct tree_t));
  if ( tree != NULL )
    tree->raiz = NULL;
  return tree;
}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){
  if(tree == NULL)
    return;
  node_destroy(tree->raiz);
  free(tree);
}

/* Função para obter da árvore o valor associado à chave key.
 * A função deve devolver uma cópia dos dados que terão de ser
 * libertados no contexto da função que chamou tree_get, ou seja, a
 * função aloca memória para armazenar uma *CÓPIA* dos dados da árvore,
 * retorna o endereço desta memória com a cópia dos dados, assumindo-se
 * que esta memória será depois libertada pelo programa que chamou
 * a função.
 * Devolve NULL em caso de erro.
 */
struct data_t *tree_get(struct tree_t *tree, char *key){
   if(tree==NULL)
     return NULL;
   struct node_t *novo = (struct node_t *)node_search(tree->raiz, key);
   if(novo==NULL)
     return NULL;
   struct data_t *dup = data_dup(novo->entry->value);
   return dup;
}

/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){
  if(tree==NULL)
    return -1;
  return node_size(tree->raiz);
}

/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.*/
int tree_put(struct tree_t *tree, char *key, struct data_t *value){
  if(tree==NULL)
    return -1;
  int was_inserted = -1;
  char *new_key = (char *)malloc((strlen(key)+1) * sizeof(char));
  strcpy(new_key, key);
  struct data_t *new_data = data_dup(value);
  struct entry_t *new_entry = entry_create(new_key, new_data);
  tree->raiz = node_insert(tree->raiz, new_entry, &was_inserted);
  return was_inserted;
}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
  int was_deleted = -1;

  if(tree!=NULL)
    tree->raiz = node_delete(tree->raiz, key, &was_deleted);
  
  return was_deleted;
}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){
    if(tree==NULL)
        return -1;
    return recursive_height(tree->raiz);
    
}

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **tree_get_keys(struct tree_t *tree){
  if(tree==NULL)
    return NULL;
  int treeSize = tree_size(tree);
  char **keys= (char**)  malloc((treeSize + 1)* sizeof(char*));
  keys[treeSize] = NULL;  
  int offset = 0;
  keys = tree_get_keys_rec(tree->raiz, keys, &offset);
  
  return keys;

}

/* Função que liberta toda a memória alocada por tree_get_keys().
 */
void tree_free_keys(char **keys){
 int i = 0;
//Descobrir size do array
  int length = 0;
  while(keys[i] != NULL){
    length++;
    i++;
  }
//Free de cada key
  for(i = 0; i < length;i++){
    if(keys[i] != NULL){
        free(keys[i]);
      }
  }
  //Free do array
  free(keys);
  keys = NULL;
}
