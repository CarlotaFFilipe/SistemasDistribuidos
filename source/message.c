// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

#include "sdmessage.pb-c.h"
#include "message-private.h"
#include "data.h"
#include "inet.h"
#include "entry.h"
#include "serialization.h"


int snd_msg_socket (struct message_t * msg, int socket){
	int res = 0;
	int wrt;
	uint32_t ser_len;
	//Tamanho da mensagem serializada
	unsigned len = message_t__get_packed_size(msg);
	//criar Buffer para serialized message + o tamanho dela
	void * buf = malloc(len + sizeof(uint32_t));
	if (buf == NULL){
		printf(stdout, "malloc error\n");//fprintf
		return -1;
	}
	//pack message
	message_t__pack(msg, buf + sizeof(uint32_t));
	//serializar o tamanho da mensagem e meter o valor no buffer
	ser_len = htonl(len);
	memcpy(buf, &ser_len, sizeof(uint32_t));
	//enviar msg nbytes a nbytes
	if((wrt = write_nbytes(socket, buf, len + sizeof(uint32_t))) < 0){
		res = -1;
	}
	//printf("\n%d bytes escritos\n", wrt);
	free(buf);
	return res;
}


struct message_t * rcv_msg_socket (int socket){
	uint32_t ser_len;
	int msg_len = 0;
	int rd = 0;
	void * buf = malloc(sizeof(uint32_t));
	if (buf == NULL){
		printf("Falta de memoria\n");
		return NULL;
	}
	rd = read_nbytes(socket, buf, sizeof(uint32_t));
	if (rd == 0){ //ligacao fechada
		free(buf);
		return NULL;
	} else if (rd == -1){ //erro
		perror("Erro na leitura");
		free(buf);
		return NULL;
	}
	//guarda o tamanho serializado da mensagem
	memcpy(&ser_len, buf, sizeof(uint32_t));
	msg_len = ntohl(ser_len);
	free(buf);
	//alloca memoria com o tamanho certo da mensagem
	buf = malloc(msg_len); //ou, em vez de free, um realloc
	if (buf == NULL){
		printf("Falta de memoria\n");
		return NULL;
	}
	rd = read_nbytes(socket, buf, msg_len);
	if (rd == -1){ //erro
		perror("Erro na leitura");
		return NULL;
	}
	//desserializa a mensagem
	struct message_t * rcv_msg = message_t__unpack(NULL, msg_len, buf);
	free(buf);
	if (rcv_msg == NULL) {
		perror("Erro ao desserializar a mensagem");
		return NULL;
	}
	return rcv_msg;
}
//le todos os bytes da mensagem
int read_nbytes (int socket, void * buf, int length){
	int offset = 0;
	while (offset < length){
		int nbytes = read(socket, buf + offset, length - offset);
		if (nbytes < 0){
			return -1;
		} else if(nbytes == 0){
			break;
		}
		offset += nbytes;
	}
	return offset;
}

//escreve todos os bytes da mensagem
int write_nbytes (int socket, void * buf, int length){
	int offset = 0;
	while (offset < length){
		int nbytes = write(socket + offset, buf + offset, length - offset);
		if (nbytes < 0){
			return -1;
		}
		offset += nbytes;
	}
	return offset;
}




////////pedidos e respostas das varias acoes////////

/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 40, c_type eh 30, meter os dados do data na mensagem
 *return 0 se a operacao corre bem e -1 caso contrario
 */
int put_request_message(struct message_t * msg, struct entry_t * entry){
	int offset=0;
	msg->opcode = 40;
	msg->c_type = 30;
	//msg->data_size = entry_to_buffer(entry, &msg->data);

	msg->data_size = entry->value->datasize;
	msg->data= malloc (msg->data_size+1);
	memcpy(msg->data, entry->value->data, msg->data_size);
	msg->data[msg->data_size]='\0';

	msg->key = strdup(entry->key);
	msg->n_keys = 1;
	return 0;
}

int put_response_message(struct message_t * msg){
	msg->c_type = 60;
	msg->data_size = 0;
	msg->data = NULL;
	return 0;
}





/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 30, c_type eh 10, meter os dados do data na mensagem
 *so enviamos 1 key
 *return 0 se a operacao corre bem e -1 caso contrario
 */
int get_request_message(struct message_t * msg, char * key){
	msg->opcode = 30;
	msg->c_type = 10;
	msg->n_keys = 1;
	msg->data = strdup(key);
	if (msg->data == NULL){
		printf("Falta de memoria\n");
		return -1;
	}
}

int get_response_message(struct message_t * msg, struct data_t * res){
	msg->opcode += 1;

	msg->c_type = 20;
	msg->data_size = res->datasize;
	msg->data = malloc(res->datasize +1);
	if (msg->data == NULL)
		return -1;
	memcpy(msg->data, res->data, res->datasize);
	msg->data[msg->data_size] = '\0';

	msg->n_keys = 0;

	return 0;
}


int key_not_found_response_message(struct message_t * msg){
	msg->opcode += 1;
	msg->c_type = 60;
	msg->data_size=0;
	msg->data=NULL;
	msg->key = NULL;
	msg->res = 0;
	msg->n_keys = 0;
}



/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 20, c_type eh 10, meter os dados do data na mensagem
 *so enviamos 1 key
 *return 0 se a operacao corre bem e -1 caso contrario
 */
int del_request_message(struct message_t * msg, char * key){
	msg->opcode = 20;
	msg->c_type = 10;
	msg->n_keys = 1;
	msg->data = strdup(key);
	if (msg->data == NULL){
		printf("Falta de memoria\n");
		return -1;
	}
	return 0;
}

int del_response_message(struct message_t * msg){
	msg->c_type= 60;
	msg->data_size = 0;
	msg->data = NULL;
	msg->n_keys = 0;
	return 0;
}




/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 10, c_type eh 60 (CT_NONE)
 */
void size_request_message(struct message_t * msg){
	msg->opcode = 10;
	msg->c_type = 60;
}
void size_response_message(struct message_t * msg, int size){
	msg->opcode += 1;
	msg->c_type = 50;
	msg->res = size;
}




/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 60, c_type eh 60 (CT_NONE)
 */
void height_request_message(struct message_t * msg){
	msg->opcode = 60;
	msg->c_type = 60;
}
void height_response_message(struct message_t * msg, int height){
	msg->opcode += 1;
	msg->c_type = 50;
	msg->res = height;
}





/*preenche a mensagem (ver protobuf) com os parametros do put
 *assim, o opcode eh 50, c_type eh 60 (CT_NONE)
 */
void get_keys_request_message(struct message_t * msg){
	msg->opcode = 50;
	msg->c_type = 60;
	msg->data_size= 0;
	msg->data = NULL;
}


int length_all_keys(char** array){
	//Descobrir size do array
	int length=0, i = 0;

	while(array[i] != NULL){
		length+= strlen(array[i])+1;
		i++;
	}
	return length;
}


int get_keys_response_message(struct message_t * msg, char ** keys, int n_keys){
	int offset = 0;
	msg->opcode += 1;
	msg->c_type = 40;
	msg->n_keys = n_keys;
	msg->data = malloc(length_all_keys(keys)+1);

	for (int i = 0; i < n_keys; i++){
		memcpy(msg->data + offset, keys[i], strlen(keys[i]));     offset += strlen(keys[i]);
		memcpy(msg->data + offset, " ", 1);                       offset +=1;
	}
	memcpy(msg->data + offset, "\0", 1);                       offset +=1;
	msg->data_size= offset;

	return 0;
}


/*preenche a mensagem (ver protobuf) com os parametros do verify
 *assim, o opcode eh 70, c_type eh 50 (CT_Result)
 */
void verify_request_message(struct message_t * msg, int op_n){
	msg->opcode = 70;
	msg->c_type = 50;
	msg->res = op_n;
}
void verify_response_message(struct message_t * msg, int op_n){
	msg->opcode += 1;
	msg->c_type = 50;
	msg->res = op_n;
}




void error_response_message(struct message_t * msg){
	msg->opcode = 99;
	msg->c_type = 60;
	msg->data_size = 0;
	msg->n_keys = 0;
  msg->data = NULL;
}
