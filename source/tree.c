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
   if(raiz == NULL || strlen(key)<=0)
     return raiz;
   int comp = strcmp(raiz->entry->key, key);
   if(comp==0){
     return raiz;
   }else if(comp <0){
     return searchFor(raiz->right, key);
   }else{
     return searchFor(raiz->left, key); 
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
   struct data_t *dup= data_dup(novo->entry->value);
//printf("\ndentro do get: %d\n", dup->data);
   return dup;

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





/*

int tree_put(struct tree_t *tree, char *key, struct data_t *value){
  if(tree==NULL || strlen(key)<=0 || value==NULL)
    return -1;
  struct node_t *corrente = tree->raiz;
  struct node_t *newnode = malloc(sizeof(struct node_t));
  newnode->entry = entry_create(key, value);
  newnode->left= newnode->right= NULL;
  //Se for o primeiro elemento da tree
  if(tree_size(tree) == 0){
    tree->raiz = newnode;
    return 0;
  }
  int cmp=0;
  while(corrente->left != NULL || corrente ->right != NULL ){
//printf("\n AQUIIIIIIIII : %s\n",key);
    cmp= strcmp(corrente->entry->key, key);
    if(cmp==0){
      entry_destroy(corrente->entry);//entry_replace?????
      newnode->left = corrente->left;
      newnode->right = corrente->right;
      corrente = newnode;
      return 0;
    }else if(cmp>0){
///quando o no ainda tem filhos ha esquerda
      if(corrente->left != NULL)
        corrente = corrente->left;
      else //quando nao tem
        break;//nao pode ser break.....
//printf("vai para a esquerda %s    \n", key);
    }else{
      if(corrente->right != NULL)
        corrente = corrente->right;
      else
        break;
//printf("vai para a direita %s    \n",key);
    }
  }
printf("\n AQUIIIIIIIII : %s\n",key);
  if(cmp>0){
    corrente->left = newnode;
//printf("\n#################meteu um novo node a esquera -  %s    ##############\n", newnode->entry->key);
  }else{ //nao nos temos de preocupar se for igual
    corrente->right = newnode;//o key-1 vai para aqui
//printf("\n#################meteu um novo node a direita -  %s    ###############\n", newnode->entry->key);
  }
  return 0;
}

*/




struct node_t * inserirEntry(struct node_t* raiz, struct entry_t *entry){
  if(raiz==NULL){
    struct node_t *node= malloc(sizeof(struct node_t*));
    node->entry=entry;
    node->left=node->right=NULL;
    raiz=node;
    return raiz;
  }
  int cmp= strcmp(raiz->entry->key, entry->key);
  if(cmp==0){
    entry_replace(raiz->entry, entry->key,entry->value);
    return raiz;
  }else if(cmp > 0){
    raiz->left = inserirEntry(raiz->left, entry);
  }else{
    raiz->right = inserirEntry(raiz->right, entry);
  }
  return raiz;
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

struct node_t *newnode = malloc(sizeof(struct node_t));
  if(tree_size(tree)==0){
    struct node_t *newnode = malloc(sizeof(struct node_t));
    newnode->entry = entry_create(key, value);
    newnode->left= newnode->right= NULL;
    tree->raiz = newnode;
    return 0;
  }
  struct entry_t *entry = entry_create(key, value);
  return (inserirEntry(tree->raiz, entry)==NULL)? -1:0;

}






void printTree(struct tree_t* tree){
printf("\n \n");
   printT(tree->raiz);
printf("\n \n");

}

void printT(struct node_t* root){
 if(root!=NULL){
   printT(root->left);
   printf("|key: %s data: %s |",root->entry->key, root->entry->value->data);
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
    return recursive_height(tree->raiz);
    
}

/*
char **tree_get_keys_rec(struct node_t *raiz){
  if(raiz!=NULL){
    tree_get_keys_rec(raiz->left);
    sprintf(keys[],"");
    tre_get_keys_rec(raiz->right);
  }
}
*/

/* Função que devolve um array de char* com a cópia de todas as keys da
 * árvore, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 */
char **tree_get_keys(struct tree_t *tree){
 /* if(tree==NULL)
    return NULL;
  return tre_get_keys_rec(tree->raiz);
*/
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