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

/* Função para criar uma nova árvore tree vazia.
 * Em caso de erro retorna NULL.
 */
struct tree_t *tree_create(){
  struct tree_t *tree = malloc(sizeof(struct tree_t));
  //tree->raiz = malloc(sizeof(tree->raiz));
  tree->raiz = NULL;
  return tree;  //acho que se ocorrer algo de errado no malloc, tree fica a null
}

/* Função para libertar toda a memória ocupada por uma árvore.
 */
void tree_destroy(struct tree_t *tree){
  if(tree == NULL)
    return;
  freeTree(tree->raiz); //deviamos fazer dentro de um while? Ja nao me lembro como fazer recursao muito bem....
  free(tree);
  tree = NULL;
}


freeTree( struct node_t *raiz ){
    if( raiz == NULL ) 
        return;
    if( raiz->left != NULL ){ //se ainda houver filhos a esquerda, apaga-los primeiro. Apaga de baixo para cima
        freeTree( raiz->left );
    }if( raiz->right != NULL ){//se ainda houver filhos a direita, apaga-los primeiro
        freeTree( raiz->right );
    }
    entry_destroy(raiz->entry); //eh uma folha, por isso liberta e destroy o conteudo
    free(raiz);
     
    return;
}


/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int tree_put(struct tree_t *tree, char *key, struct data_t *value){
  if(tree==NULL)
    return -1;
  if(tree->raiz== NULL){
    struct node_t *novo = (struct node_t *)malloc(sizeof(struct node_t));
    memcpy(novo->entry->key, key);
    novo->left= novo->left= NULL;
    return 1;
  }///////////verificar outra vez se left e right estão bem
  if(entry_compare(tree->raiz->entry->key, key)>=0)
    tree->raiz->left = tree_put(tree->raiz->left, key, value);
  else
    tree->raiz->right = tree_put(tree->raiz->right, key, value);

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

}



//////////////////////////////////////////////////////////////////////
//Ainda nao acabei o tree_del
//////////////////////////////////////////////////////////////////////


/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
  if(tree==NULL)
    return -1;
  if(tree->raiz == NULL)
    return 0;
  else if(entry_compare(key, tree->raiz->entry->key)==-1)
    tree->raiz->left = tree_del(tree->raiz->left, key);
  else if(entry_compare(key, tree->raiz->entry->key)==1)
    tree->raiz->right = tree_del(tree->raiz->right, key);

  else{
    if(tree->raiz->left == NULL){

    }
  }
}

/*


struct node_t * minValueNode(struct node_t *node){
   struct node_t *current = node;
   while (current && current->left != NULL)
      current = current->left;
   return current;
}
struct node* deleteNode(struct node* root, int key){
   if (root == NULL) return root;
   if (key < root->key)
      root->left = deleteNode(root->left, key);
   else if (key > root->key)
      root->right = deleteNode(root->right, key);
   else{
      if (root->left == NULL){
         struct node *temp = root->right;
         free(root);
         return temp;
      }
      else if (root->right == NULL){
         struct node *temp = root->left;
         free(root);
         return temp;
      }
      struct node* temp = minValueNode(root->right);
      root->key = temp->key;
      root->right = deleteNode(root->right, temp->key);
   }
   return root;
}





*/



/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){
    if(tree==NULL || tree->raiz==NULL)
        return 0;
    else
        return (tree_size(tree->raiz->left) +1 + tree_size(tree->raiz->right));  //este +1 podem pensar que eh o node em que estamos
}


/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){
    if(tree==NULL)
        return;
    if(tree->raiz==NULL)
        return 0;
    else{
        int altura_esquerda = tree_height(tree->raiz->left);
        int altura_direita = tree_height(tree->raiz->right);
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
char **tree_get_keys(struct tree_t *tree){

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