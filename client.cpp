/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 2048 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void *recv_messages(void *vargp) {
    //recv in thread
    while (1)
    {
        int sockfd = *((int *)vargp);
        char *buf = (char *)malloc(MAXDATASIZE);
        if(recv(sockfd, buf, MAXDATASIZE, 0) == -1) {
                perror("recv");
                exit(1);
        }
        //if buf starts with OUTPUT, print the rest of the string
        if (strncmp(buf, "OUTPUT", 6) == 0) {
            printf("\n%s\n", buf);
        }
       
    }
     return NULL;
}
int main(int argc, char *argv[])
{
    int sockfd;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    //start recv thread
    pthread_t thread;
    int *sockfd_ptr = (int *)malloc(sizeof(int));
    *sockfd_ptr = sockfd;
    pthread_create(&thread, NULL, recv_messages, (void *)sockfd_ptr);
    //send input to server
    while(1) {
        printf("client: enter message to send to server: ");
        memset(buf, 0, MAXDATASIZE);
        fgets(buf, MAXDATASIZE, stdin);
        if (send(sockfd, buf, strlen(buf), 0) == -1) {
            perror("send");
            exit(1);
        }
        //receive message from server to a thread

    }
    close(sockfd);

    return 0;
}