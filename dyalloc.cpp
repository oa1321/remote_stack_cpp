#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct mem_block {
    int start;
    int size;
    struct mem_block *next;
};

void* dynmic_alloc(int size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Error: malloc failed\n");
        exit(1);
    }
    return ptr;
}

void* dynmic_calloc(int size) {
    void *ptr = calloc(size, 1);
    if (ptr == NULL) {
        printf("Error: calloc failed\n");
        exit(1);
    }
    return ptr;
}

void* dynmic_free(void *ptr) {
    free(ptr);
    return NULL;
}