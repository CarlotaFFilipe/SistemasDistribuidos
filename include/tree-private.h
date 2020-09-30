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


#endif
