# Grupo21
# Carlota Filipe n51027
# Leonor Candeias n51057
# Mafalda Paço n51

CC=gcc
INCLUDE = include/
OBJ = object/
BIN = binary/
SRC = source/
LIB = lib/
FLAG = gcc -g -w -Wall -I include/ -I lib/ -c
LINKFLAGS= ld -r

all: proto data.o entry.o tree.o serialization.o message.o network_client.o client_stub.o tree_cliente.o network_server.o tree_skel.o tree_server.o sdmessage.pb-c.o client-lib.o tree_client tree_server

proto: 
	protoc --c_out=./lib/ sdmessage.proto


data.o: $(INCLUDE)data.h
	$(FLAG) $(SRC)data.c -o $(OBJ)data.o

entry.o: $(INCLUDE)entry.h $(INCLUDE)data.h
	$(FLAG) $(SRC)entry.c -o $(OBJ)entry.o

tree.o: $(INCLUDE)tree.h $(INCLUDE)tree-private.h $(INCLUDE)data.h $(INCLUDE)entry.h
	$(FLAG) $(SRC)tree.c -o $(OBJ)tree.o

serialization.o: $(INCLUDE)serialization.h $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h $(INCLUDE)tree-private.h
	$(FLAG) $(SRC)serialization.c -o $(OBJ)serialization.o

###############fase 2
message.o: $(INCLUDE)inet.h $(INCLUDE)data.h $(INCLUDE)serialization.h $(INCLUDE)message-private.h $(LIB)sdmessage.pb-c.h
	$(FLAG) $(SRC)message.c -o $(OBJ)message.o

tree_cliente.o:  $(INCLUDE)data.h  $(INCLUDE)entry.h  $(INCLUDE)tree.h $(INCLUDE)client_stub-private.h $(INCLUDE)network_client.h $(LIB)sdmessage.pb-c.h
	$(FLAG) $(SRC)tree_client.c -o $(OBJ)tree_client.o

network_client.o: $(INCLUDE)network_client.h $(INCLUDE)client_stub.h $(LIB)sdmessage.pb-c.h $(INCLUDE)message-private.h $(INCLUDE)client_stub-private.h
	$(FLAG) $(SRC)network_client.c -o $(OBJ)network_client.o

client_stub.o: $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h $(INCLUDE)serialization.h $(INCLUDE)client_stub.h $(INCLUDE)client_stub-private.h $(INCLUDE)network_client.h $(INCLUDE)message-private.h $(LIB)sdmessage.pb-c.h
	$(FLAG) $(SRC)client_stub.c -o $(OBJ)client_stub.o

tree_skel.o: $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h $(INCLUDE)serialization.h $(INCLUDE)tree_skel.h $(INCLUDE)message-private.h  $(LIB)sdmessage.pb-c.h
	$(FLAG) $(SRC)tree_skel.c -o $(OBJ)tree_skel.o

tree_server.o: $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h $(INCLUDE)network_server.h
	$(FLAG) $(SRC)tree_server.c -o $(OBJ)tree_server.o

network_server.o: $(INCLUDE)tree_skel.h $(INCLUDE)network_server.h $(INCLUDE)message-private.h $(LIB)sdmessage.pb-c.h
	$(FLAG) $(SRC)network_server.c -o $(OBJ)network_server.o

sdmessage.pb-c.o: $(LIB)sdmessage.pb-c.h
	$(FLAG) $(LIB)sdmessage.pb-c.c -o $(OBJ)sdmessage.pb-c.o

##tree.o
client-lib.o: 
	$(LINKFLAGS) $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o $(OBJ)serialization.o $(OBJ)message.o $(OBJ)client_stub.o $(OBJ)network_client.o $(OBJ)sdmessage.pb-c.o -o $(OBJ)client-lib.o 

tree_client: $(OBJ)client-lib.o $(OBJ)tree_client.o
	$(CC) -L/usr/local/lib -lprotobuf-c $(OBJ)client-lib.o $(OBJ)tree_client.o /usr/local/lib/libprotobuf-c.a -o $(BIN)tree_client
#message.o
tree_server:
	 $(CC) -L/usr/local/lib -lprotobuf-c $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o $(OBJ)serialization.o $(OBJ)message.o $(OBJ)tree_skel.o $(OBJ)network_server.o $(OBJ)tree_server.o $(OBJ)sdmessage.pb-c.o /usr/local/lib/libprotobuf-c.a -o $(BIN)tree_server 






run:
#	./binary/test_data
#	valgrind --leak-check=yes ./binary/test_data
#	./binary/test_entry
#	valgrind --leak-check=yes ./binary/test_entry
#	./binary/test_tree
#	valgrind --leak-check=yes ./binary/test_tree
#	./binary/test_serialization
#	valgrind --leak-check=yes ./binary/test_serialization
##	./binary/tree_server 12345 2
##	./binary/tree_client 127.0.0.1:12345
clean:
	rm $(OBJ)*.o
	rm binary/*
