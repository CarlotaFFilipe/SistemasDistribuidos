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

all : data.o entry.o tree.o serialization.o test_data.o test_entry.o test_tree.o test_serialization.o test_data test_entry test_tree test_serialization run

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
sdmessage.pb-c.o: $(LIB)sdmessage.pb-c.h
	$(FLAG) $(LIB)sdmessage.pb-c.c -o $(OBJ)sdmessage.pb-c.o




run:
#	./binary/test_data
#	valgrind --leak-check=yes ./binary/test_data
#	./binary/test_entry
#	valgrind --leak-check=yes ./binary/test_entry
#	./binary/test_tree
#	valgrind --leak-check=yes ./binary/test_tree
#	./binary/test_serialization
#	valgrind --leak-check=yes ./binary/test_serialization
##	./binary/table_server 12345 2
##	./binary/table_client 127.0.0.1:12345
clean:
	rm $(OBJ)*.o
	rm binary/*
