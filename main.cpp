#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.hpp"

void std_output_use(){
    printf(" i am a main func\n");
    //using create_stack
    struct stack *s = (struct stack *)create_stack("SERVER STACK");
    char input[MAX_DATA];
    while (1) {
        printf("INPUT: ");
        fgets(input, MAX_DATA, stdin);
        if (strcmp(input, "exit\n") == 0) {
            break;
        }
        stack_command(s, input);
    }
    free_stack(s);
}

int main(){
    std_output_use();
    return 0;
}