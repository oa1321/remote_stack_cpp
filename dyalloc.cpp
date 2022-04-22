#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>


#define ALIGNMENT 4
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct mem_block {
    size_t size;
    struct mem_block *next;
    struct mem_block *prev;
    int free;
    void *ptr;
    char data[1];
} mem_block, *mem_block_ptr;

#define HEADER_SIZE sizeof(mem_block)

static mem_block_ptr head = NULL;

mem_block_ptr get_mem_block(void *ptr) {
    return (mem_block_ptr)((char *)ptr - HEADER_SIZE);
}

int is_valid_pointer(void *ptr) {
    if (head && ptr > head && ptr < sbrk(0)) {
        return (ptr == (get_mem_block(ptr)->ptr));
    }
    return 0;
}

void split_block(mem_block_ptr block, size_t size) {
    mem_block_ptr new_block = (mem_block_ptr)(block->data + size);
    new_block->size = block->size - HEADER_SIZE;
    new_block->next = block->next;
    if (new_block->next != NULL) {
        new_block->next->prev = new_block;
    }
    new_block->prev = block;
    new_block->free = 1;
    new_block->ptr = new_block->data;
    block->size = size;
    block->next = new_block;
}

mem_block_ptr find_free_block(mem_block_ptr *prev, size_t size) {
    mem_block_ptr block = head;
    while (block && !(block->free && block->size >= size)) {
        *prev = block;
        block = block->next;
    }
    return block;
}

mem_block_ptr extend_heap(mem_block_ptr block, size_t size) {
    mem_block_ptr new_block;
    new_block = (mem_block_ptr)sbrk(0);
    if (sbrk(HEADER_SIZE + size) == (void *)-1) {
        return NULL;
    }
    new_block->size = size;
    new_block->next = NULL;
    new_block->prev = block;
    new_block->ptr = new_block->data;
    if (block != NULL) {
        block->next = new_block;
    }
    new_block->free = 0;
    return new_block;
}

void *dynmic_alloc(size_t size) {
    mem_block_ptr block, prev;
    size_t new_size = ALIGN(size);
    if (head != NULL) {
        prev = head;
        block = find_free_block(&prev, new_size);
        if (block != NULL) {
            if ((block->size - new_size) >= (HEADER_SIZE + 4)) {
                split_block(block, new_size);
            }
            block->free = 0;
        } else {
            block = extend_heap(prev, new_size);
            if (block == NULL) {
                return NULL;
            }
        }
    } else {
        block = extend_heap(NULL, new_size);
        if (block == NULL) {
            return NULL;
        }
        head = block;
    }
    return block->data;
}

mem_block_ptr merge_blocks(mem_block_ptr block) {
    mem_block_ptr next = block->next;
    if (next != NULL && next->free) {
        block->size += next->size + HEADER_SIZE;
        block->next = next->next;
        if (block->next != NULL) {
            block->next->prev = block;
        }
    }
    return block;
}

void* dynmic_calloc(size_t n_items,size_t size){
    void *ptr = dynmic_alloc(n_items*size);
    if(ptr){
        memset(ptr,0,n_items*size);
    }
    return ptr;
}

void dynmic_free(void *ptr) {
    mem_block_ptr block;
    if (is_valid_pointer(ptr)) {
        block = get_mem_block(ptr);
        block->free = 1;
        if (block->prev != NULL && block->prev->free) {
            block = merge_blocks(block->prev);
        }
        if (block->next != NULL) {
            block = merge_blocks(block);
        } else {
            if (block->prev != NULL) {
                block->prev->next = NULL;
            } else {
                head = NULL;
            }
            brk(block);
        }
    }
}
