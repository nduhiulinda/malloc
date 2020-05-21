#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "spinlock.h"
 
#define BLOCK_NOT_FOUND -1
 
/* 
 * Global lock object.  You should use this global lock for any locking you need to do.
 */
#ifdef __riscv
volatile lock_t malloc_lock = {.riscv_lock = 0};
#else
volatile lock_t malloc_lock = {.pthread_lock = PTHREAD_MUTEX_INITIALIZER};
#endif
 
#define ADD_BYTES(base_addr, num_bytes) (((void *)(base_addr)) + (num_bytes))
 
typedef struct _block_info_t {
    unsigned int block_size;
    // void *block;
    unsigned int allocated;
} block_info_t;
 
typedef struct _heap_header_t {
    unsigned int heap_size;
    // unsigned int free_heap_size;
    block_info_t* first_block;
    // bool in_use_f[0];
} heap_header_t ;
 
 
/**Prints 
 * 
 */
void print_debug_heap_header(heap_header_t *header) {
#ifdef PRINT_DEBUG
    printf("heap starts at addr %p\n"   // C printing trick.
           "heap_size = %d\n",            // Notice: no commas between lines
           header, header->heap_size); 
 
    printf("block[%d] =[%p]\n", 0, header->first_block);
 
#endif
}
 
void print_debug_block_header(block_info_t *header) {
#ifdef PRINT_DEBUG
    printf("block starts at addr %p\n"   // C printing trick.
           "block_size = %d\n",            // Notice: no commas between lines
           header, header->block_size); 
 
    printf("block[%d] =[%d,%d]\n", 0, header->block_size, header->allocated);
 
#endif
}
 
void print_debug_alloc(void *block_addr) {
#ifdef PRINT_DEBUG
    printf("will give user block beginning @: %p\n", block_addr);
#endif
}
 
void print_debug_sizeof(void *block_addr) {
#ifdef PRINT_DEBUG
    printf("size of heap_header @: %ldu\n", sizeof(block_addr));
#endif
}
 
 
block_info_t* find_block(heap_header_t *header, void *block, int block_size) {
    if (block==NULL){
        return NULL;
    }
    block_info_t *curr_block =header->first_block;
    void* i = curr_block;
    // searching through blocks, looking for one given
    while (i<(ADD_BYTES(header,header->heap_size))){
       if (curr_block==block) { // found it!
           return curr_block;
       }
       i+=curr_block->block_size;
       curr_block=ADD_BYTES(curr_block, curr_block->block_size);
       if ((uintptr_t)curr_block%ALIGNMENT!=0){
            int rem = (uintptr_t)curr_block%ALIGNMENT;
            curr_block=ADD_BYTES(curr_block,(ALIGNMENT-rem));
        }
    }
    return NULL;
}

void *align(void *heap, void *block){
    heap_header_t *header = (heap_header_t *)heap;
    if ((uintptr_t)block%ALIGNMENT!=0){
        int rem = (uintptr_t)block%ALIGNMENT;
        if (ADD_BYTES(block,(ALIGNMENT-rem)) < ADD_BYTES(header,header->heap_size)){
            block=ADD_BYTES(block,(ALIGNMENT-rem));
        }else{
            block=NULL;
        }
    }
    return block;
}
 
/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First checks if heap is 8-byte aligned. Aligns it if it's not.
 * Initializes heap metadata and first block metadata
 */
  int hl_init(void *heap, unsigned int heap_size) {
    
    if (heap_size < MIN_HEAP_SIZE){
        return FAILURE;
    }
    //ensure heap is 8-byte aligned
    if ((uintptr_t)heap%ALIGNMENT!=0){
        int rem = (uintptr_t)heap%ALIGNMENT;
        heap=ADD_BYTES(heap, (ALIGNMENT-rem));
    }
    //initialize heap metadata
    heap_header_t *header = (heap_header_t *)heap;
    header->heap_size = heap_size;
    //initialize first block metadata
    block_info_t *block_head = ADD_BYTES(heap, sizeof(heap_header_t));
    block_head=align(heap,block_head);
    header->first_block = block_head;
    block_head->allocated=0;
    block_head->block_size=heap_size-(sizeof(heap_header_t));
 
    return SUCCESS;
}
 
/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 * 1. We assign in the 1st block
 * 2. We assign other to block
 * 3. We have no free space
 * 
 */
// The size of the header (~8 bytes) is not the size of the entire heap
void *hl_alloc(void *heap, unsigned int block_size) {
    if (block_size ==0){
        return NULL;
    }
    heap_header_t *header = (heap_header_t *)heap;
    int i = sizeof(heap_header_t);
    int j = sizeof(block_info_t);
        block_info_t *curr_block =header->first_block;
        while (i+block_size+j<header->heap_size){
            if (!(curr_block->allocated) && j+block_size<=curr_block->block_size){
                int old_size = curr_block->block_size;
                curr_block->block_size = block_size + j;
                curr_block->allocated = 1;
                block_info_t *new_block = ADD_BYTES(curr_block, curr_block->block_size);
                new_block=align(heap,new_block);
                new_block->block_size= old_size - curr_block->block_size;
                new_block->allocated=0;
                return ADD_BYTES(curr_block, sizeof(block_info_t));
            }
            i+=curr_block->block_size;
            curr_block=ADD_BYTES(curr_block, curr_block->block_size);
            curr_block=align(heap,curr_block);
        }
    return FAILURE;
}
 
/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void hl_release(void *heap, void *block) {
    if (block==NULL){
        return;
    }
    block = ADD_BYTES(block, -sizeof(block_info_t));
    heap_header_t *header = (heap_header_t *)heap;
    block_info_t *main_block=(block_info_t *)block;
    block_info_t* finder = find_block(header,main_block,main_block->block_size);
    if (finder!=NULL) {
        finder->allocated=0;
        block_info_t *next_block = ADD_BYTES(finder , finder->block_size);
        next_block=align(heap,next_block);
        next_block=find_block(header, next_block, next_block->block_size);
        if (next_block!=NULL && next_block->allocated==0){
            int new_size=finder->block_size+next_block->block_size;
            next_block->block_size=0;
            next_block->allocated=0;
            next_block=NULL;
            finder->block_size=new_size;
    }
    }
}
 
/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {
     if (new_size==0){
        return FAILURE;
    }
    if (block==0){
        return hl_alloc(heap,new_size);
    }
    block = ADD_BYTES(block, -sizeof(block_info_t));
    heap_header_t *header = (heap_header_t *)heap;
    block_info_t *main_block=(block_info_t *)block;
    block_info_t* finder = find_block(header,main_block,main_block->block_size);
    new_size=new_size+sizeof(block_info_t);
    if (finder->block_size>=new_size){
        finder->block_size = new_size;
        return ADD_BYTES(finder, sizeof(block_info_t));;
    }else{
        block_info_t* new_block=hl_alloc(heap, new_size);
        if (new_block!=NULL){
            new_block->allocated=1;
            new_block->block_size=new_size;
            memmove(ADD_BYTES(new_block,sizeof(block_info_t)),ADD_BYTES(finder,sizeof(block_info_t)), sizeof(char)*new_size);
            hl_release(heap,finder); 
            return ADD_BYTES(new_block, sizeof(block_info_t)); 
        }
    }
    return FAILURE;
}
