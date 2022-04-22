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
#include <assert.h>
#include "stack.hpp"
#include "dyalloc.hpp"
//test the stack_command function
void test_free_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    assert(free_stack(stack_server) == 0);
}
void test_push_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
    assert(push(stack_server, data) == 0);
}
void test_pop_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
    assert(pop(stack_server, data) == -1);
    assert(push(stack_server, data) == 0);
    assert(pop(stack_server, data) == 0);
}

void test_top_stack() {
    //create a stack
    char *stack_name = "test_stack1";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //free the stack and check return val is 0(succsess) with assert
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
    assert(top(stack_server, data) == -1);
    assert(push(stack_server, data) == 0);
    assert(top(stack_server, data) == 0);
}
void test_stack_command() {
    //create a stack
    char *stack_name = "test_stack2";
    struct stack *stack_server = (struct stack *)create_stack(stack_name);
    //try to push a value
    push(stack_server, "1");
    //try to push a value
    push(stack_server, "2");
    //try to push a value
    push(stack_server, "3");
    //check if values are in the stack with top
    char *data = (char *)malloc(sizeof(char) * MAX_DATA);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "3") == 0);
    //pop the value
    pop(stack_server, data);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "2") == 0);
    //pop the value
    pop(stack_server, data);
    top(stack_server, data);
    //check if data is 3
    printf("%s\n", data);
    assert(strcmp(data, "1") == 0);
    //pop the value
    pop(stack_server, data);
}
int main(){
    test_stack_command();
    test_free_stack();
    test_push_stack();
    test_pop_stack();
    test_top_stack();
    return 0;
}