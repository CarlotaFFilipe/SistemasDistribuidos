// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507





//TODO: falta a parte dos sinais

#include "tree_skel.h"
//#include "network_server.h"
#include "message-private.h"
#include "sdmessage.pb-c.h"

#include <signal.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//variaveis
struct sockaddr_in server,client;
int socket_servidor,socket_cliente;
socklen_t size_client;
int option_name;


/* Função para preparar uma socket de receção de pedidos de ligação
 * num determinado porto.
 * Retornar descritor do socket (OK) ou -1 (erro).
 */
int network_server_init(short port){
  //Cria o socket do servidor
  if((socket_servidor= socket(AF_INET,SOCK_STREAM,0))<0){
    printf("Erro ao criar o socket");
    return-1;
  }
  //Preparar server para fazer bind
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  //Fazer bind a um porto usado anteriormente e registado pelo kernel como ainda ocupado
  if(setsockopt(socket_servidor,SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option_name,sizeof(option_name)) < 0){
	printf("Erro no setsockopt\n");
	close(socket_servidor);
	return -1;
  }
  
  // Faz bind
  if (bind(socket_servidor, (struct sockaddr *) &server, sizeof(server)) < 0){
    printf("Erro no bind\n");
    close(socket_servidor);
    return -1;
  }

  // Faz listen
  if (listen(socket_servidor, 0) < 0){
    printf("Erro ao executar listen\n");
    close(socket_servidor);
    return -1;
  }

  printf("Servidor a espera de dados\n");

  return socket_servidor;

}

/* Esta função deve:
 * - Aceitar uma conexão de um cliente;
 * - Receber uma mensagem usando a função network_receive;
 * - Entregar a mensagem de-serializada ao skeleton para ser processada;
 * - Esperar a resposta do skeleton;
 * - Enviar a resposta ao cliente usando a função network_send.
 */
int network_main_loop(int listening_socket){
  //Aceitar uma conexão de um cliente
  if((socket_cliente = accept(listening_socket, (struct sockaddr *) &client, &size_client)) == -1)
	    return -1;

  
  while(true){
    struct message_t *msg = network_receive(socket_cliente);
    if(msg == NULL){
      close(socket_cliente);
      printf("Erro ao receber mensagem\n");
      return -1;
    }else{
      if(invoke(msg) == -1){
        printf("Erro ao construir resposta\n");
        close(socket_cliente);
        return -1;
      }
      if(network_send(socket_cliente,msg) == -1){
        printf("Erro ao enviar mensagem\n");
	close(socket_cliente);
        return -1;
      }
    }
  }
  close(socket_cliente);
  return 0;
}

/* Esta função deve:
 * - Ler os bytes da rede, a partir do client_socket indicado;
 * - De-serializar estes bytes e construir a mensagem com o pedido,
 *   reservando a memória necessária para a estrutura message_t.
 */
struct message_t *network_receive(int client_socket){
  return rcv_msg_socket(client_socket);
}

/* Esta função deve:
 * - Serializar a mensagem de resposta contida em msg;
 * - Libertar a memória ocupada por esta mensagem;
 * - Enviar a mensagem serializada, através do client_socket.
 */
int network_send(int client_socket, struct message_t *msg){
  int res = snd_msg_socket(msg, client_socket);
  message_t__free_unpacked(msg, NULL);
  return res;
}

/* A função network_server_close() liberta os recursos alocados por
 * network_server_init().
 */
int network_server_close(){
  int result;
  if((result= close(socket_servidor)) == -1)
    return -1;
  //close do client_socket eh feito no loop
  return result;
}
