#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dyalloc.hpp"
#define MAX_DATA 2048

int split_command(char org_str[MAX_DATA], size_t from, char new_str[MAX_DATA]){
    //split a string into two parts
    //return -1 if error
    //return 0 if success
    int i = 0;
    while (from < strlen(org_str)) {
        new_str[i] = org_str[from];
        i++;
        from++;
    }
    new_str[i] = '\0';
    return 0;
}

int split_command_by_first_space(char org_str[MAX_DATA], char new_str[MAX_DATA]){
    //split a string into two parts by first space assumes the string is end with \n
    //return -1 if error
    //return 0 if success
    size_t i = 0;
    //check size is more than 0 - needed for the next check of \n
    if (strlen(org_str) <= 0) {
        return -1;
    }
    //check if last char is \n - not good because fgets will add \n and we want to remove it
    if (org_str[strlen(org_str) - 1] == '\n') {
        org_str[strlen(org_str) - 1] = '\0';
    }
    while (org_str[i] != ' ' && i < strlen(org_str)) {
        
        new_str[i] = org_str[i];
        i++;
    }
    new_str[i] = '\0';
    return 0;
}

/*
|------------------------------------|
|======= stack implementation =======|
|------------------------------------|

struct of a node 
struct of a stack
a push function - push a string (max len is 1024) - return -1 if empty
a pop function - pop a string (max len is 1024) - return -1 if empty
a top function - return the top string (max len is 1024) - return -1 if empty
a dynmic_free_stack function - dynmic_free the stack nodes and set the stack to be empty
*/

struct node {
    //a node contains a string and a pointer to the next node 
    char *data;
    struct node *next;
};

struct stack {
    //a stack contains a pointer to the top node and size of the stack
    int size;
    char *stack_name;
    struct node *head;
};

int push(struct stack *s, char *data) {
    //push a string to the stack top
    //return -1 if empty
    //return 0 if success
    struct node *new_node = (struct node *)dynmic_alloc(sizeof(struct node));
    if (new_node == NULL) {
        printf("ERROR: dynmic_alloc failed - PUSH OPERTION\n");
        return -1;
    }
    new_node->data = (char *)dynmic_alloc(sizeof(char) * strlen(data));
    if (new_node->data == NULL) {
        printf("ERROR: dynmic_alloc failed - PUSH OPERTION\n");
        return -1;
    }
    strcpy(new_node->data, data);
    new_node->next = s->head;
    s->head = new_node;
    s->size++;
    return 0;
}

int pop(struct stack *s, char *data) {
    //pop a string from the stack top
    //return -1 if empty
    //return 0 if success
    if (s->size == 0) {
        printf("ERROR: STACK IS EMPTY - POP OPERTION\n");
        return -1;
    }
    struct node *temp = s->head;
    strcpy(data, temp->data);
    s->head = temp->next;
    dynmic_free(temp);
    s->size--;
    return 0;
}

int top(struct stack *s, char *data) {
    //return the top node string
    //return -1 if empty
    //return 0 if success
    if (s->size == 0) {
        printf("ERROR: STACK IS EMPTY - TOP OPERTION\n");
        return -1;
    }
    struct node *temp = s->head;
    strcpy(data, temp->data);
    printf("OUTPUT: %s\n", data);
    return 0;
}
int free_stack(struct stack *s) {
    //dynmic_free the stack nodes and set the stack to be empty
    //stack it self is not dynmic_freed because it can be reused or not on heap
    //need to dynmic_free it manually
    struct node *temp = s->head;
    while (temp != NULL) {
        struct node *next = temp->next;
        dynmic_free(temp->data);
        dynmic_free(temp);
        temp = next;
    }
    dynmic_free(s->stack_name);
    s->size = 0;
    s->head = NULL;
    return 0;
}
int print_stack(struct stack *s) {
    //print the stack
    struct node *temp = s->head;
    printf("%s CONTENT:\n", s->stack_name);
    printf(" _______________\n");
    printf("|   stack-top   |\n\n");
  
    while (temp != NULL) {
        printf("| %s |\n", temp->data);
        temp = temp->next;
    }
     printf(" _______________\n");
    printf("|   stack-end   |\n\n");
 
    return 0;
}
int stack_command(struct stack *s, char * data){
    //stack command
    //return -1 if error
    //return 0 if success
    char *command = (char *)dynmic_alloc(sizeof(char) * MAX_DATA);
    char *command_data = (char *)dynmic_alloc(sizeof(char) * MAX_DATA);
    split_command_by_first_space(data, command);
    split_command(data, strlen(command) + 1, command_data);
    //check if command is PUSH
    if (strcmp(command, "PUSH") == 0) {
        push(s, command_data);
         strcpy(data, command_data);
        return 0;
    }
    //check if command is POP
    else if (strcmp(command, "POP") == 0) {
        pop(s, command_data);
         strcpy(data, command_data);
        return 0;
    }
    //check if command is TOP
    else if (strcmp(command, "TOP") == 0) {
        top(s, command_data);
        strcpy(data, "OUTPUT: ");
        strcat(data, command_data);
        return 0;
    }
    //check if command is dynmic_free
    else if (strcmp(command, "dynmic_free") == 0) {
        free_stack(s);
         strcpy(data, command_data);
        return 0;
    }
    //check if command is PRINT
    else if (strcmp(command, "PRINT") == 0) {
        print_stack(s);
        strcpy(data, command_data);
        return 0;
    }
    return 0;
}   
void* create_stack(char *stack_name){
    //create a stack
    struct stack *s = (struct stack *)dynmic_alloc(sizeof(struct stack));
    s->size = 0;
    s->stack_name = (char *)dynmic_alloc(sizeof(char) * strlen(stack_name));
    strcpy(s->stack_name, stack_name);
    s->head = NULL;
    return s;
}
/*
|------------------------------------|
|======= notes interface  =======    |
|------------------------------------|

1. change all arrays to dynamic array ( dynmic_alloc for strat ) - string in nodes mostly  - done
2. try to look for a way to use brk and sbrk to allocate memory 
3. write a function to do dynmic_alloc calloc and dynmic_free

how to create a cpp exe
g++ -Wall -o stack stack.cpp
*/
