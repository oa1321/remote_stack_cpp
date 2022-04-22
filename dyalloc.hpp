#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct mem_block {
    size_t size;
    struct mem_block *next;
    struct mem_block *prev;
    int free;
    void *ptr;
    char data[1];
} mem_block, *mem_block_ptr;

void* dynmic_alloc(size_t size);
void* dynmic_calloc(size_t n_items,size_t size);
void* dynmic_free(void *ptr);


