// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


#include "client_stub.h"
#include "client_stub-private.h"
#include "data.h"
#include "entry.h"
#include "tree_skel.h"

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


int test_input(int argc, char **argv){
	int res= 0;
	if(argc != 2){
		printf("tree_client <ip servidor>:<porta servidor>\n");
		printf("Exemplo de uso: ./binary/tree_client 127.0.0.1:12345\n");
		res= -1;
	}
	//ver se existe o caracter ':'
	else if(strstr(argv[1],":") == NULL){
		printf("Uso: ./tree_client <hostname>:<port>\n");
		printf("Exemplo de uso: ./tree_client 127.0.0.1:12345\n");
		res = -1;
	}
	return 0;
}

void client_handler (struct rtree_t *rt){
	rtree_disconnect(rt);
	exit(0);
}

int main(int argc, char **argv){

	struct rtree_t *rt;
	char* terminal;
	char* corrente;
	bool ctrl_z = false;
	int tamanho = 0;

	if(test_input(argc, argv)==-1)
		return -1;

	//conectar com o porto e ip
	rt = rtree_connect(argv[1]);
	if(rt == NULL)
		return -1;

	signal(SIGINT, client_handler);
	signal(SIGQUIT, client_handler);
	//Evita crash devido ao fecho da ligação
	signal(SIGPIPE, SIG_IGN);


	//ver quais as acoes

	terminal = (char *) malloc(MAX_MSG * sizeof(char));

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
		//caso paramentros a mais num certo comando, o codigo irah ignora-los
		if(strcmp(corrente,"quit") == 0){
			ctrl_z = true;
			continue;
		}


		else if(strcmp(corrente,"size") == 0){
			int size = rtree_size(rt);
			if(size == -1)
				printf("Erro no calculo do tamanho da arvore.\n");
			else
				printf("Numero de entries na tree: %d\n",size);
			continue;
		}


		else if(strcmp(corrente,"height") == 0){
			int height = rtree_height(rt);
			if(height == -1)
				printf("Erro no calculo da altura da arvore.\n");
			else
				printf("Tamanho da tree: %d\n",height);
			continue;
		}


		else if(strcmp(corrente,"del") == 0){
			corrente = strtok(NULL, " ");
			if(corrente == NULL){
				printf("Comando del mal escrito, por favor faca o input desta maneira: del <key>\n");
				continue;
			}
			int last_assigned =rtree_del(rt,corrente);
			if(last_assigned == -1)
				printf("Erro na eliminacao da entry.\n");
			else
				printf("A chave foi %s eliminada.\n Last_assigned igual a %d\n",corrente, last_assigned);
			continue;
		}


		else if(strcmp(corrente,"get") == 0){
			corrente = strtok(NULL," ");
			if(corrente == NULL){
				printf("Comando get mal escrito, por favor faca o input desta maneira: get <key>\n");
				continue;
			}
			struct data_t *data = rtree_get(rt,corrente);
			if(data == NULL)
				printf("Nao existe essa key.\n");
			else{
				printf("A data na key %s eh %s\n",corrente, data->data);
				data_destroy(data);
			}
			continue;
		}



		else if(strcmp(corrente,"put") == 0){
			corrente = strtok(NULL, " ");
			if(corrente == NULL){
				printf("Comando put mal escrito, por favor faca o input desta maneira: put <key> <data>\n");
				continue;
			}
			char *key = corrente;
			corrente = strtok(NULL, "\0");
			if(corrente == NULL){
				printf("Comando put mal escrito, por favor faca o input desta maneira: put <key> <data>\n");
				continue;
			}
			struct data_t *data = data_create2(strlen(corrente),corrente);
			struct entry_t *entry = entry_create(key,data);
			int last_assigned = rtree_put(rt,entry);
			//if (last_assigned == -1)
			//	printf("Erro na insersao da entry.\n");
			//else
			printf("A chave foi posta com sucesso. Last_assigned igual a %d\n", last_assigned);
			free(data);
			continue;
		}



		else if(strcmp(corrente,"getkeys") == 0){
			int i=0;
			char **keys = rtree_get_keys(rt);
			if (keys == NULL)
				printf("Erro em encontrar todas as keys.\n");
			else{
				printf("Existem estas keys na tree: %s\n", keys);
				//rtree_free_keys(keys);
				free(keys);
			}
			continue;
		}
		else if(strcmp(corrente,"verify")==0){
			corrente = strtok(NULL," ");
			if(corrente == NULL){
				printf("Comando verify mal escrito, por favor faca o input desta maneira: verify <op_n>\n");
				continue;
			}
			int result = rtree_verify(rt, atoi(corrente));
			if (result ==-1)
				printf("Erro no verify.\n");
			if(result == 1)
				printf("Essa operacao ja foi executada.\n");
			else
				printf("Essa operacao esta na fila de espera ou nao existe.\n");
		}


		else{
			printf("Comando inválido. Exemplo de comandos:\n size\n height \n del <key>\n get <key> \n put <key> <data> \n getkeys\n verify <op_n>\n quit\n");
			continue;
		}
	}
	//free(corrente);
	free(terminal);
	return rtree_disconnect(rt);

}