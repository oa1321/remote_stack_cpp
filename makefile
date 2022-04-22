CC = g++
FLAGS = -Wall -g

all: main server client

client: client.cpp
	$(CC) $(FLAGS) -o client client.cpp	-lpthread

server: stack_server.o stack.o
	$(CC) $(FLAGS) -o server stack_server.o stack.o -lpthread

stack_server.o: stack_server.cpp
	$(CC) $(FLAGS) -c stack_server.cpp

main: main.o stack.o
	$(CC) $(FLAGS) -o main main.o stack.o

main.o: main.cpp
	$(CC) $(FLAGS) -c main.cpp

stack: stack.o
	$(CC) $(FLAGS) -o stack stack.o

stack.o: stack.cpp
	$(CC) $(FLAGS) -c stack.cpp

clean:
	rm -f *.o client main server