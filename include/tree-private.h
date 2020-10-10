// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"

struct node_t {
    struct entry_t *entry;
    struct node_t *left, *right; 
};

struct tree_t {
  struct node_t *raiz;
};

struct node_t * node_create(struct entry_t *entry);

void node_destroy( struct node_t *raiz );

struct node_t * node_insert(struct node_t* raiz, struct entry_t * entry, int *pwas_inserted);

struct node_t * node_delete(struct node_t *raiz, char *key, int *was_deleted);

struct node_t* node_search(struct node_t *raiz, char* key);

struct node_t * node_min_value(struct node_t *node);

int node_size(struct node_t *raiz);

void printTree(struct tree_t* tree);

void printT(struct node_t* root);

int recursive_height(struct node_t *raiz);

char **tree_get_keys_rec(struct node_t *raiz, char** algo,int *offset);

#endif
