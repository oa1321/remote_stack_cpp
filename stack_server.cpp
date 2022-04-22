/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "stack.hpp"
#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
pthread_mutex_t mutex;
//the stack
struct stack *stack_server;

struct stack_listen_args {
    char *arg1;
	pthread_mutex_t *arg2;
	int sockfd;
};


void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

void *do_stack_command_thread_safe(void *vargp) {
	struct stack_listen_args *args = (struct stack_listen_args *)vargp;
	char *buffer = args->arg1;
	pthread_mutex_t *mutex = args->arg2;
	int sockfd = args->sockfd;
	//lock mutex
	pthread_mutex_lock(mutex);

	stack_command(stack_server, buffer);
	//if output staers with OUTPUT then send it to the client
	//send the output to the client
	
	int n = send(sockfd, buffer, strlen(buffer), 0);
	if (n <= 0) perror("ERROR writing to socket");
	
	//unlock mutex
	pthread_mutex_unlock(mutex);
	return NULL;
}

void *recv_stack_command(void *vargp){
	int sockfd = *(int *)vargp;
	char buffer[MAX_DATA];
	int n;
	while (1) {
		n = read(sockfd, buffer, MAX_DATA);
		if (n < 0) {
			printf("ERROR: reading from socket");
		}
		if (n == 0) {
			printf("DEBUG Client disconnected");
			break;
		}
		buffer[n] = '\0';
		//alloc memory for the stack_listen_args
		struct stack_listen_args *args = (struct stack_listen_args *)malloc(sizeof(struct stack_listen_args));
		args->arg1 = buffer;
		args->arg2 = &mutex;
		args->sockfd = sockfd;
		pthread_t thread;
		pthread_create(&thread, NULL, do_stack_command_thread_safe, args);

		if (strcmp(buffer, "exit\n") == 0) {
			break;
		}
	}
	close(sockfd);
	pthread_exit(NULL);
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		printf("ERROR: getaddrinfo\n");
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			printf("ERROR: server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			printf("ERROR: setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			printf("ERROR: server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		printf("DEBUG: server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		printf("ERROR: listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		printf("ERROR: sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");
	//create a stack
	stack_server = (struct stack *)create_stack("SERVER STACK");
	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			printf("ERROR: accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("DEBUG: server: got connection from %s\n", s);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, recv_stack_command, (void*)&new_fd);
	
	}
    
	return 0;
}
/*
|------------------------------------|
|======= notes interface  =======    |
|------------------------------------|
1. add a locking mechanism to the stack functions

*/