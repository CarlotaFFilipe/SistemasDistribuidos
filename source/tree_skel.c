// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

//TODO implementar o last_assigned e o op_count

#include "sdmessage.pb-c.h"
#include "tree_skel.h"
#include "message-private.h"
#include "entry.h"
#include "data.h"
#include "tree.h"
#include "serialization.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tree_t *tree;

int last_assigned= 0;
int op_count= 0;

/* Inicia o skeleton da árvore.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). 
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int tree_skel_init(){
    tree=tree_create();
    if(tree == NULL)
        return -1;
    return 0;
}

/* Liberta toda a memória e recursos alocados pela função tree_skel_init.
 */
void tree_skel_destroy(){
    if(tree!=NULL)
        tree_destroy(tree);

}

/* Executa uma operação na árvore (indicada pelo opcode contido em msg)
 * e utiliza a mesma estrutura message_t para devolver o resultado.
 * Retorna 0 (OK) ou -1 (erro, por exemplo, árvore nao incializada)
*/
int invoke(struct message_t *msg){
    if(tree == NULL || msg == NULL){
        return -1;
    }

    if(msg->opcode == 10){  //A mensagem foi composta incorretamente
        if(msg->c_type!=60){
	    printf("A mensagem size foi composta incorretamente\n");
	    return -1;
	}
        size_response_message(msg, tree_size(tree));
        return 0;

    } else if(msg->opcode == 20){  //A mensagem foi composta incorretamente
        if(msg->c_type!=10 || msg->n_keys != 1){
	    printf("A mensagem del foi composta incorretamente\n");
	    return -1;
	}
        if(tree_del(tree, msg->data)==-1){
            msg->opcode = 99;
        } else{
            msg->opcode +=1;
	}        
	del_response_message(msg, msg->data);
	return 0;

    } else if(msg->opcode == 30){  //A mensagem foi composta incorretamente
        if(msg->c_type!=10 || msg->n_keys != 1){
	    printf("A mensagem get foi composta incorretamente\n");
	    return -1;
	}
        struct data_t *data=tree_get(tree, msg->data);
        free(msg->data);
	if(data ==NULL){
	    key_not_found_response_message(msg);
        } else{
	     if(get_response_message(msg, data)==-1)
             return -1;
        }
        data_destroy(data);
        return 0;

    } else if(msg->opcode == 40){  //A mensagem foi composta incorretamente
        if(msg->c_type!=30 || msg->n_keys != 1){
	    printf("A mensagem put foi composta incorretamente\n");
	    return -1;
	}
        //com o serialization
        //struct entry_t *entry = buffer_to_entry(msg->data, msg->data_size);
        struct data_t *data = data_create2(msg->data_size, msg->data);

        int put = tree_put(tree, msg->key, data);
        if(put == -1){
            msg->opcode = 99;
        }else{
            msg->opcode += 1;
        }
        put_response_message(msg);
    		data_destroy(data);
        return 0;

    } else if(msg->opcode == 50){ //getkeys
//A mensagem foi composta incorretamente
        if(msg->c_type!=60){
	    printf("A mensagem getkeys foi composta incorretamente\n");
	    return -1;
	}
        int size = tree_size(tree);
        char** keys = tree_get_keys(tree);
        if(keys == NULL){
            return -1;
	}
        get_keys_response_message(msg, keys, size);
        tree_free_keys(keys);
        return 0;

    } else if(msg->opcode == 60){//height
//A mensagem foi composta incorretamente
        if(msg->c_type!=60){
	    printf("A mensagem height foi composta incorretamente\n");
	     return -1;
	}
        int tamanho = tree_height(tree);
        height_response_message(tree, tamanho);
        return 0;

    } else{//opcode 99, o servidor nao deve receber este opcode
	error_response_message(msg);
        return -1;
    }

}

/* Verifica se a operação identificada por op_n foi executada.*/
int verify(int op_n){
    return op_n < op_count;
}