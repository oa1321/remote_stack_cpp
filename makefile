CC = g++
FLAGS = -Wall -g

all: main server client test

client: client.cpp
	$(CC) $(FLAGS) -o client client.cpp	-lpthread

server: server.o stack.o dyalloc.o
	$(CC) $(FLAGS) -o server server.o stack.o dyalloc.o -lpthread

server.o: server.cpp
	$(CC) $(FLAGS) -c server.cpp

main: main.o stack.o dyalloc.o
	$(CC) $(FLAGS) -o main main.o stack.o dyalloc.o

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

test: test.o stack.o dyalloc.o
	$(CC) $(FLAGS) -o test test.o stack.o dyalloc.o

test.o: test.cpp
	$(CC) $(FLAGS) -c test.cpp

stack: stack.o dyalloc.o
	$(CC) $(FLAGS) -o stack stack.o dyalloc.o

stack.o: stack.cpp
	$(CC) $(FLAGS) -c stack.cpp

dyalloc.o: dyalloc.cpp
	$(CC) $(FLAGS) -c dyalloc.cpp
clean:
	rm -f *.o client main server test