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



void freeTree( struct node_t *raiz ){
    if( raiz == NULL ) 
        return;
    if( raiz->left != NULL ){ //se ainda houver filhos a esquerda, apaga-los primeiro. Apaga de baixo para cima
        freeTree( raiz->left );
    }if( raiz->right != NULL ){//se ainda houver filhos a direita, apaga-los primeiro
        freeTree( raiz->right );
    }
    entry_destroy(raiz->entry); //eh uma folha, por isso liberta e destroy o conteudo
    free(raiz);
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












struct node_t* searchFor(struct node_t *raiz, char* key){
   if(raiz ==NULL || strlen(key)<=0){
     return NULL;
}
   int comp = strcmp(raiz->entry->key, key);
   if(comp==0){
     return raiz;
   }else if(comp <1){
     raiz->right = searchFor(raiz->right, key);
   }else{
     raiz->left = searchFor(raiz->left, key); 
   }
//Nao existe essa key na tree
   return NULL;
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
   struct node_t *novo = (struct node_t *) searchFor(tree->raiz, key);
   if(novo==NULL)
     return NULL;
   return novo->entry->value;

}












int true_size(struct node_t *raiz){
  if(raiz==NULL)
    return 0;
  else 
    return (true_size(raiz->left) + 1 + true_size(raiz->right));//este +1 podem pensar que eh o node em que estamos

}

/* Função que devolve o número de elementos contidos na árvore.
 */
int tree_size(struct tree_t *tree){
  if(tree==NULL)
    return -1;
  return true_size(tree->raiz);
}







/* Função para adicionar um par chave-valor à árvore.
 * Os dados de entrada desta função deverão ser copiados, ou seja, a
 * função vai *COPIAR* a key (string) e os dados para um novo espaço de
 * memória que tem de ser reservado. Se a key já existir na árvore,
 * a função tem de substituir a entrada existente pela nova, fazendo
 * a necessária gestão da memória para armazenar os novos dados.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */

int isLeaf(struct node_t *raiz){
  return (raiz->left==NULL && raiz->right==NULL)? 1:0 ;
}

int tree_put(struct tree_t *tree, char *key, struct data_t *value){
  if(tree==NULL || strlen(key)<=0 || value==NULL)
    return -1;
  struct node_t *corrente = tree->raiz;
  //ainda nao existe nenhuma entry com essa key
  if(tree_get(tree,key)==NULL){
    //criar novo node
    struct node_t *newnode = malloc(sizeof(struct node_t));
    newnode->entry = entry_create(key, value);
    newnode->left= newnode->right= NULL;
    //Se for o primeiro elemento da tree
    if(tree_size(tree) == 0){
      tree->raiz = newnode;
      return 0;
    }
    //andar para a frente, sem recursao
int i=0;
    while(corrente->left != NULL || corrente -> right != NULL ){
i++;
printf("entra no while %d -  %s    \n", i, key);
      int comp = strcmp(corrente->entry->key,key);
      if(comp>0){
        corrente = corrente->left;
printf("vai para a esquerda %d -  %s    \n", i, key);
      }else{
        corrente = corrente->right;
printf("vai para a direita %d -  %s    \n", i, key);
      }
    }
    //chegou a folha da aevore,
    int comp = strcmp(corrente->entry->key,key);
    if(comp > 0){
      corrente->left = newnode;
printf("meteu um novo node a esquera -  %s    \n", newnode->entry->key);
    }else{
      corrente->right = newnode;
printf("meteu um novo node a direita -  %s    \n", newnode->entry->key);
    }
    return 0;
  }else{//ja existe entry com esta key, temos de a substituir
    while(corrente != NULL){
      int comp = strcmp(corrente->entry->key,key);
      if(comp == 0){
      entry_destroy(corrente->entry);
      corrente->entry = entry_create(key, value);;
        return 0;
      }else if(comp>0)
        corrente = corrente->left;
      else 
        corrente = corrente->right;
    }
  }
  return -1;
}






void printTree(struct tree_t* tree){
printf("\n");
   printT(tree->raiz);
printf("\n");
}

void printT(struct node_t* root){
 if(root!=NULL){
   printT(root->left);
   printf(" %s ",root->entry->key);
   printT(root->right);
 }
}





//////////////////////////////////////////////////////////////////////
//Ainda nao acabei o tree_del
//////////////////////////////////////////////////////////////////////


struct node_t * minValueNode(struct node_t *node){
   struct node_t *current = node;
   while (current && current->left != NULL)
      current = current->left;
   return current;
}

int deleteNode(struct node_t *raiz, char *key){
  if (raiz == NULL) 
    return 0;
  int comp = strcmp(raiz->entry->key, key);

  if (comp > 1)
    raiz->left = deleteNode(raiz->left, key);
  else if (comp < -1)
    raiz->right = deleteNode(raiz->right, key);
  else{
    if (raiz->left == NULL){
      struct node_t *temp = raiz->right;
      free(raiz);
      return 0;
    }
    else if (raiz->right == NULL){
      struct node_t *temp = raiz->left;
      free(raiz);
      return 0;
    }
    struct node_t *temp = minValueNode(raiz->right);
    raiz->entry->key = temp->entry->key;
    raiz->right = deleteNode(raiz->right, temp->entry->key);
  }
   return 0;
}

/* Função para remover um elemento da árvore, indicado pela chave key,
 * libertando toda a memória alocada na respetiva operação tree_put.
 * Retorna 0 (ok) ou -1 (key not found).
 */
int tree_del(struct tree_t *tree, char *key){
  if(tree==NULL)
    return -1;

  return deleteNode(tree->raiz,key);
}





int recursive_height(struct node_t *raiz){
   if(raiz==NULL)
        return 0;
    else{
        int altura_esquerda = tree_height(raiz->left);
        int altura_direita = tree_height(raiz->right);
        if(altura_esquerda > altura_direita)
            return altura_esquerda +1;
        else
            return altura_direita + 1;
    }
}

/* Função que devolve a altura da árvore.
 */
int tree_height(struct tree_t *tree){
    if(tree==NULL)
        return -1;
    return recursive_height;
    
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