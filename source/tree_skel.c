// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507

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
    if(tree == NULL || msg == NULL)
        return -1;

    if(msg->opcode == 10){
        size_response_message(msg, tree_size(tree));
        return 0;


    }else if(msg->opcode== 20){
        if(tree_del(tree, msg->data)==-1)
            msg->opcode = 99;
        else
            msg->opcode +=1;
        del_request_message(msg, msg->data);
        return 0;


    }else if(msg->opcode == 30){
        struct data_t *data= tree_get(tree, msg->data);
        free(msg->data);
        if(data == NULL){
            error_response_message(msg);
        }else{
            if(get_response_message(msg, data)==-1)
                return -1;
        
            data_destroy(data);
        }
        return 0;

    }else if(msg->opcode == 40){
        //com o serialization
        //struct entry_t *entry = buffer_to_entry(msg->data, msg->data_size);
//entry_create()
        struct entry_t *entry = malloc(sizeof(struct entry_t));
        char *corrente;
        corrente  = strtok(msg->data," ");
        entry->key = corrente;
        corrente = strtok(NULL, " ");
        struct data_t *data = data_create(atoi(corrente));
        corrente = strtok(NULL, " ");
        data->data =corrente;
        entry->value = data;


        int put = tree_put(tree, entry->key, entry->value);
        if(put == -1){
            msg->opcode = 99;
        }else{
            msg->opcode += 1;
        }
        put_response_message(msg);
    //entry_destroy(entry);
    free(data);
    free(entry);
        return 0;


    }else if(msg->opcode == 50){ //getkeys
        int size = tree_size(tree);
        char** keys = tree_get_keys(tree);
        if(keys == NULL)
            return -1;
        get_keys_response_message(msg, keys, size);
        tree_free_keys(keys);
printf("\n\n   %s\n\n", msg->data);
        return 0;

    }else if(msg->opcode == 60){//height
        int tamanho = tree_height(tree);
printf("\n\n   %d\n\n", tamanho);
        height_response_message(tree, tamanho);
        return 0;

    }else{//opcode 99, o servidor nao deve receber este opcode
        return -1;
    }

}