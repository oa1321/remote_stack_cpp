#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct mem_block {
    int start;
    int size;
    struct mem_block *next;
};

void* dynmic_alloc(int size);
void* dynmic_calloc(int size);
void* dynmic_free(void *ptr);


