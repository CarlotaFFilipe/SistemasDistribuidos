OBJ_dir = object/
SRC_dir = source/
BIN_dir = binary/
INC_dir = include/
LIB_dir = lib/

#Alterar de acordo com a localizacao do protobuf nesta maquina
PROTOC_dir=/usr/local/

CC = gcc
CFLAGS = -Wall -g -O2 -I $(INC_dir)
LDFLAGS = ${PROTOC_dir}lib/libprotobuf-c.a
OBJS_lib = sdmessage.pb-c.o client_stub.o network_client.o message.o table.o list.o data.o entry.o
OBJS_client = sdmessage.pb-c.o table-client.o network_client.o client_stub.o message.o data.o entry.o list.o table.o
OBJS_server = sdmessage.pb-c.o table-server.o network_server.o table_skel.o message.o data.o entry.o list.o table.o


all: table-server table-client client-lib.o

client-lib.o: $(OBJS_lib)
	ld -r $(addprefix $(OBJ_dir),$(OBJS_lib)) -o $(LIB_dir)client-lib.o

table-client: $(OBJS_client)
	$(CC) $(addprefix $(OBJ_dir),$(OBJS_client)) $(LDFLAGS) -o $(BIN_dir)table-client

table-server: $(OBJS_server)
	$(CC) $(addprefix $(OBJ_dir),$(OBJS_server)) $(LDFLAGS) -o $(BIN_dir)table-server

%.o: $(SRC_dir)%.c $($@)
	$(CC) $(CFLAGS) -c -o $(OBJ_dir)$@ $<

sdmessage.pb-c.o:
	${PROTOC_dir}bin/protoc-c --c_out=. sdmessage.proto
	@mv -f sdmessage.pb-c.c $(SRC_dir)
	@mv -f sdmessage.pb-c.h $(INC_dir)
	$(CC) -c -I $(INC_dir) $(SRC_dir)sdmessage.pb-c.c
	@mv -f sdmessage.pb-c.o $(OBJ_dir)

clean:
	@rm -f $(OBJ_dir)*
	@rm -f $(LIB_dir)*
	@rm -f $(BIN_dir)*
	@rm -f $(INC_dir)/sdmessage.pb-c.h
	@rm -f $(SRC_dir)/sdmessage.pb-c.c
