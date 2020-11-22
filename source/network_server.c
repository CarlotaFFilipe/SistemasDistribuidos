// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057 
// Mafalda Paço n53507


#include "tree_skel.h"
#include "network_server.h"
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


#include <poll.h>
#include <fcntl.h>
#define NFDESC 10 // Numero de sockets (uma para listening)
#define TIMEOUT 50 // em milisegundos

//variaveis
struct sockaddr_in server,client;
int socket_servidor,socket_cliente;
socklen_t size_client;
int option_name, nfds, kfds, i;
//socklen_t size_client;

struct pollfd connections[NFDESC]; // Estrutura para file descriptors das sockets das ligacoes

//struct message_t *network_receive(int client_socket);
//int network_send(int client_socket, struct message_t *msg);


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
	if (listen(socket_servidor, 10) < 0){
		printf("Erro ao executar listen\n");
		close(socket_servidor);
		return -1;
	}

	printf("Servidor a espera de dados\n");

	return socket_servidor;

}

/*
 * atualiza o array das connections apos uma delas ter sido fechada
 * por exemplo com a operacao quit
 */
void reorganize_connection_array(int i, int size){
	for(int j = i; j < size; j++){
		memcpy(&(connections[j]), &(connections[j+1]), sizeof(struct pollfd));
	}
}

/* Codigo baseado no enunciado e exercicios fornecidos pelo professor
 */
int network_main_loop(int listening_socket){
	bool is_connected = true;
	size_client = sizeof(struct sockaddr);

	for (i = 0; i < NFDESC; i++){
		connections[i].fd = -1;    // poll ignora estruturas com fd < 0
	}

	connections[0].fd = socket_servidor; 
	connections[0].events = POLLIN;  // Vamos esperar por dados disponiveis para leitura
	nfds = 1; // numero de file descriptors

	// Retorna assim que exista um evento ou que TIMEOUT expire. * FUNCAO POLL *.
	while ((kfds = poll(connections, nfds, 10)) >= 0){ // kfds == 0 significa timeout sem eventos

		if (kfds > 0){ // kfds e o numero de descritores com evento ou erro
			if ((connections[0].revents & POLLIN) && (nfds < NFDESC)){ 
				if ((connections[nfds].fd = accept(connections[0].fd, (struct sockaddr *) &socket_cliente, &size_client)) > 0){
					connections[nfds].events = POLLIN;
					nfds++;
				}
			}
			for (i = 1; i < nfds; i++){ // Todas as ligacoes
				is_connected = true;
				if (connections[i].revents & POLLIN) {
					struct message_t *msg = network_receive(connections[i].fd );
					if(msg == NULL){
						printf(" Socket %d fechado.\n", connections[i].fd);
						is_connected =false;
					} else{
						if(invoke(msg) == -1){
							printf("A sair\n");
							is_connected = false;
						}
						else if(network_send(connections[i].fd ,msg) == -1){
							printf("Erro ao enviar mensagem. Socket %d fechado.\n", connections[i].fd);
							is_connected = false;
						}
					}
					if ( is_connected == false )
					{
						close(connections[i].fd);
						reorganize_connection_array(i, nfds);
						nfds--;
						i--;

					}
				}
			}
		}
	}


	// fechar ligacao
	close(socket_cliente);
	network_server_close();
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
