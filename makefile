CC=g++
CFLAGS=-std=c++11 -I -Wall -lpthread -lstdc++ 
DEPS = pastry.h md5.h
OBJ = pastryNode.o pastryNodeServer.o md5.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pastry: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)