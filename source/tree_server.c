// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include "client_stub-private.h"
#include "network_server.h"
#include "tree-private.h"
#include "tree.h"
#include "data.h"
#include "entry.h"
#include "message-private.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>


//handler para os sinais SIGINT e SIGQUIT
void quit_handler (int sig){
  tree_skel_destroy();
  network_server_close();
}


int testInput(int argc){
  if (argc != 2){
    printf("Uso: ./tree_server <port>\n");
    printf("Exemplo de uso: ./tree_server 12345\n");
    return -1;
  }
  return 0;
}


int main(int argc, char **argv){

	signal(SIGINT, quit_handler);
  signal(SIGQUIT, quit_handler);
  //Evita crash devido ao fecho da ligação
  signal(SIGPIPE, SIG_IGN);


   // Verifica se foi passado algum argumento
  if (testInput(argc) == -1)
    return -1;
  //buscar e guardar o porto.
  int socket;
  if((socket=network_server_init(atoi(argv[1])))==-1)
    return -1;
  if(tree_skel_init() == -1)
    return -1;
  //network_main_loop
  int result;
  if((result= network_main_loop(socket))==-1)
    return -1;
  
  //detroir arvore e acabar a ligacao
  tree_skel_destroy();
  network_server_close();
    
}