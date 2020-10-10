# Grupo21
# Carlota Filipe n51027
# Leonor Candeias n51057
# Mafalda Paço n51

CC=gcc
INCLUDE = include/
OBJ = object/
BIN = binary/
SRC = source/
#FLAG = gcc -g -w -Wall -I include/ -c
FLAG = gcc -g -I include/ -c

all : data.o entry.o tree.o serialization.o test_data.o test_entry.o test_tree.o test_serialization.o test_data test_entry test_tree test_serialization run

data.o: $(INCLUDE)data.h
	$(FLAG) $(SRC)data.c -o $(OBJ)data.o

entry.o: $(INCLUDE)entry.h $(INCLUDE)data.h
	$(FLAG) $(SRC)entry.c -o $(OBJ)entry.o

tree.o: $(INCLUDE)tree.h $(INCLUDE)tree-private.h $(INCLUDE)data.h $(INCLUDE)entry.h
	$(FLAG) $(SRC)tree.c -o $(OBJ)tree.o

serialization.o: $(INCLUDE)serialization.h $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h $(INCLUDE)tree-private.h
	$(FLAG) $(SRC)serialization.c -o $(OBJ)serialization.o

test_data.o: $(INCLUDE)data.h
	$(FLAG) $(SRC)test_data.c -o $(OBJ)test_data.o

test_entry.o: $(INCLUDE)data.h $(INCLUDE)entry.h
	$(FLAG) $(SRC)test_entry.c -o $(OBJ)test_entry.o

test_tree.o: $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)tree.h
	$(FLAG) $(SRC)test_tree.c -o $(OBJ)test_tree.o

test_serialization.o: $(INCLUDE)data.h $(INCLUDE)entry.h $(INCLUDE)serialization.h
	$(FLAG) $(SRC)test_serialization.c -o $(OBJ)test_serialization.o

test_data: $(OBJ)test_data.o $(OBJ)data.o
	$(CC) $(OBJ)test_data.o $(OBJ)data.o -o binary/test_data

test_entry: $(OBJ)test_entry.o $(OBJ)data.o $(OBJ)entry.o
	$(CC) $(OBJ)test_entry.o $(OBJ)data.o $(OBJ)entry.o -o binary/test_entry

test_tree: $(OBJ)test_tree.o  $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o
	$(CC) $(OBJ)test_tree.o  $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o -o binary/test_tree

test_serialization: $(OBJ)test_serialization.o  $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o $(OBJ)serialization.o
	$(CC) $(OBJ)test_serialization.o  $(OBJ)data.o $(OBJ)entry.o $(OBJ)tree.o $(OBJ)serialization.o -o binary/test_serialization

run:
	./binary/test_data
#	valgrind --leak-check=yes ./binary/test_data
	./binary/test_entry
#	valgrind --leak-check=yes ./binary/test_entry
	./binary/test_tree
#	valgrind --leak-check=yes ./binary/test_tree
	./binary/test_serialization
#	valgrind --leak-check=yes ./binary/test_serialization

clean:
	rm $(OBJ)*.o
	rm binary/*
