// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


#include "client_stub.h"
#include "client_stub-private.h"
#include "data.h"
#include "entry.h"
/*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>*/
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#define MAX_MSG 2048

int test_input(int argc){
  if(argc != 2){
    printf("tree_client <ip servidor>:<porta servidor>\n");
    printf("Exemplo de uso: ./binary/tree_client 127.0.0.1:12345\n");
    return -1;
  }
  return 0;
}

int main(int argc, char **argv){

  struct rtree_t *rt;
  char* terminal;
  char* corrente;
  bool ctrl_z = false;
  int tamanho = 0;

  if(test_input(argc)==-1)
    return -1;
//ver se existe o caracter ':'
  if(strstr(argv[1],":") == NULL){
    printf("Uso: ./tree_client <hostname>:<port>\n");
    printf("Exemplo de uso: ./tree_client 127.0.0.1:12345\n");
    return -1;
  }
//conectar com o porto e ip
  rt = rtree_connect(argv[1]);
  if(rt == NULL)
    return -1;

  //ver quais as acoes
  
  corrente = malloc(MAX_MSG);
  terminal = malloc(MAX_MSG*sizeof(char));

  while(!ctrl_z){
    printf("==>");
    fgets(terminal, MAX_MSG, stdin);
    terminal[strlen(terminal) -1] = '\0';

    tamanho = strlen(terminal);
    if(tamanho>1)
      corrente = strtok(terminal," ");
    else
      corrente = terminal;
//ver todas as acoes
    if(strcmp(corrente,"quit") == 0){
      ctrl_z = true;
      continue;
    }
    else if(strcmp(corrente,"size") == 0){
      printf("Numero de entries na tree: %d\n",rtree_size(rt));
      continue;
    }
    else if(strcmp(corrente,"height") == 0){
      printf("Tamanho da tree: %d\n",rtree_height(rt));
      continue;
    }
    else if(strcmp(corrente,"del") == 0){
      corrente = strtok(NULL, " ");
      rtree_del(rt,corrente);
      printf("A chave foi %s eliminada.\n", corrente);
      continue;
    }
    else if(strcmp(corrente,"get") == 0){
      corrente = strtok(NULL," ");
      struct data_t *data = rtree_get(rt,corrente);
      printf("O value de %s eh %s.\n", corrente,data->data);
//data_destroy?????
      continue;
    }
    else if(strcmp(corrente,"put") == 0){
      corrente = strtok(NULL, " ");
      char *key = corrente;
      corrente = strtok(NULL, " ");
      struct data_t *data = data_create2(strlen(corrente),corrente);
      struct entry_t *entry = entry_create(key,data);
      rtree_put(rt,entry);
      //free(entry->key);
      free(entry);//destroy
      printf("A chave foi posta com sucesso\n");
      continue;
    }
    else if(strcmp(corrente,"getkeys") == 0){
      char *keys=rtree_get_keys(rt);
      printf("Existem estas keys na tabela: %s\n", keys);
      continue;
    }
    else{
      printf("Comando inválido. Exemplo de comandos:\n size\n height \n del <key>\n get <key> \n put <key> <data> \n getkeys\n quit\n");
      continue;
    }
  }
  //free(corrente);
  free(terminal);
  return rtree_disconnect(rt);

}