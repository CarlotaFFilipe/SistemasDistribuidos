// Grupo21
// Carlota Filipe n51027
// Leonor Candeias n51057
// Mafalda Paço n53507


FASE 1:


FASE 2:

 ** TO RUN **
		correr o servidor num terminal:
			./binary/tree-server 12345
							OU
			valgrind --leak-check=yes ./binary/tree-server 12345
    
		correr um cliente noutro terminal:
			./binary/tree-client 127.0.0.1:12345
							OU
			valgrind --leak-check=yes ./binary/tree-client 127.0.0.1:12345


 ** ClEAN **
		correr no terminal
			make clean

 ** Problemas **
		-A operacao height, quando eh chamada ocorre problemas no message_t__get_packed_size;
    -rtree_get_keys do stub_client é uma string em vez de um array de strings;
		-Fecho do servidor;
		

 ** MAKEFILE **

- Necessário alterar a localização do valor PROTOC_dir no makefile de acordo com
  a localização da instalação do protobuf nesta máquina.


 ** PROTO **
- Foram adicionados novos campos à mensagem:
    -n_keys     (numero de chaves em keys)
    -key       (todas as chaves)
    -res     (resultado da operação size e height)

