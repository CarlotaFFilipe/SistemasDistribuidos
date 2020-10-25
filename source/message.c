


#include "sdmessage.pb-c.h"
#include "message-private.h"
#include "data.h"
#include "inet.h"
#include "entry.h"
#include "serialization.h"

char* arrayString_to_string ( char ** array, int n_keys, int length );



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
  //le o tamanho da mensagem
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
  msg->opcode = 40;
  msg->c_type = 30;
  msg->data_size = entry->value->datasize;
  //msg->value = malloc(sizeof(char* ));
  //Transformar entry para string e huardar tamanho em data_size
  msg->data_size = entry_to_buffer(entry,&msg->data);
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
  msg->keys = strdup(key);
  if (msg->keys == NULL){
    printf("Falta de memoria\n");
    return -1;
  }
  return 0;
}

int get_response_message(struct message_t * msg, struct data_t * res){
  msg->opcode += 1;
  msg->c_type = 20;

  return 0;
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
  msg->keys = strdup(key);
  if (msg->keys == NULL){
    printf("Falta de memoria\n");
    return -1;
  }
  return 0;
}

int del_response_message(struct message_t * msg, char * key){
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
  msg->result = size;
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
  msg->result = height;
}



/*preenche a mensagem (ver protobuf) com os parametros do put
*assim, o opcode eh 50, c_type eh 60 (CT_NONE)
*/
void get_keys_request_message(struct message_t * msg){
  msg->opcode = 50;
  msg->c_type = 60;
}

int get_keys_response_message(struct message_t * msg, char ** keys, int n_keys){
  msg->opcode += 1;
  msg->c_type = 40;
  msg->n_keys = n_keys;
  if(n_keys>0){
    int sum = 0;
    for (int i = 0; i < n_keys; i++)
      sum += (strlen(keys[i]) + 1);
    msg->data= arrayString_to_string(keys, n_keys, sum);
  }

//meter no final '\0'
  //memcpy(msg->data + sum, "\0", 1);
  return 0;
}

char* arrayString_to_string ( char ** array, int n_keys, int length ) {
  //rever o tamanho
  char * string = malloc(length);
  //char * string = malloc(sizeof(char *));
  for (int i = 0; i < n_keys; i++) {
    strcat(string,array[i]); //memcpy??
    strcat(string, " ");
  }
  return string;
}







void error_response_message(struct message_t * msg){
  msg->opcode = 99;
  msg->c_type = 60;
  msg->data_size = 0;
  msg->n_keys = 0;
}
