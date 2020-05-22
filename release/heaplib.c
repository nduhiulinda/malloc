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
       printf("(find block )curr block size:%d\n",curr_block->block_size);
       fflush(NULL);
       i+=curr_block->block_size;
       curr_block=ADD_BYTES(curr_block, curr_block->block_size);
       if ((uintptr_t)curr_block%ALIGNMENT!=0){
            int rem = (uintptr_t)curr_block%ALIGNMENT;
            curr_block=ADD_BYTES(curr_block,(ALIGNMENT-rem));
        }
    }
    return NULL;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * First checks if heap is 8-byte aligned. Aligns it if it's not.
 * Initializes heap metadata and first block metadata
 */
  int hl_init(void *heap, unsigned int heap_size) {
    mutex_lock(&malloc_lock);
    if (heap_size < MIN_HEAP_SIZE){
        mutex_unlock(&malloc_lock);
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

    if ((uintptr_t)block_head%ALIGNMENT!=0){
        int rem = (uintptr_t)block_head%ALIGNMENT;
        block_head=ADD_BYTES(block_head,(ALIGNMENT-rem));
    }
    
    header->first_block = block_head;
    block_head->allocated=0;
    block_head->block_size=heap_size-(sizeof(heap_header_t));
    mutex_unlock(&malloc_lock);
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
    mutex_lock(&malloc_lock);
    printf("in alloc start\n");
    if (block_size ==0){
        mutex_unlock(&malloc_lock);
        return NULL;
    }
    heap_header_t *header = (heap_header_t *)heap;
    int i = sizeof(heap_header_t);
    int j = sizeof(block_info_t);
        block_info_t *curr_block =header->first_block;
        printf("(alloc initial)i+block_size+j:%d\n",i+block_size+j);
        printf("(alloc initial)heap size:%d\n",header->heap_size);
        while (i+block_size+j<header->heap_size){
            printf("(alloc while loop)i:%d\n",i);
            printf("curr_block:%p\n",curr_block);
            printf("curr_block->allocated:%d\n",curr_block->allocated);
            printf("curr_block->block_size:%d\n",curr_block->block_size);
            printf("(alloc while loop)j+block_size:%d\n",j+block_size);
            if (!(curr_block->allocated) && j+block_size<curr_block->block_size){
                int old_size = curr_block->block_size;
                curr_block->block_size = block_size + j;
                curr_block->allocated = 1;
                block_info_t *new_block = ADD_BYTES(curr_block, curr_block->block_size);
                printf("(alloc found block to alloc)new_block:%p\n",new_block);
                printf("(alloc found block to alloc)curr_block->block_size:%d\n",curr_block->block_size);
                if ((uintptr_t)new_block%ALIGNMENT!=0){
                  int rem = (uintptr_t)new_block%ALIGNMENT;
                  new_block=ADD_BYTES(new_block,(ALIGNMENT-rem));
                }
                new_block->block_size= old_size - curr_block->block_size;
                printf("(alloc divide block to alloc)new_block->block_size:%d\n",new_block->block_size);
                new_block->allocated=0;
                mutex_unlock(&malloc_lock);
                return ADD_BYTES(curr_block, sizeof(block_info_t));
            }
            printf("(alloc not found block to alloc)curr_block:%p\n",curr_block);
            printf("curr_block->block_size:%d\n",curr_block->block_size);
            i+=curr_block->block_size;
            curr_block=ADD_BYTES(curr_block, curr_block->block_size);
            if ((uintptr_t)curr_block%ALIGNMENT!=0){
                int rem = (uintptr_t)curr_block%ALIGNMENT;
                curr_block=ADD_BYTES(curr_block,(ALIGNMENT-rem));
            }
            printf("(alloc check next block to alloc)curr_block:%p\n",curr_block);
        }
    mutex_unlock(&malloc_lock);
    return FAILURE;
}

void *hl_alloc2(void *heap, unsigned int block_size) {
    printf("in alloc start\n");
    if (block_size ==0){
        return NULL;
    }
    heap_header_t *header = (heap_header_t *)heap;
    int i = sizeof(heap_header_t);
    int j = sizeof(block_info_t);
        block_info_t *curr_block =header->first_block;
        printf("(alloc initial)i+block_size+j:%d\n",i+block_size+j);
        printf("(alloc initial)heap size:%d\n",header->heap_size);
        while (i+block_size+j<header->heap_size){
            printf("(alloc while loop)i:%d\n",i);
            printf("curr_block:%p\n",curr_block);
            printf("curr_block->allocated:%d\n",curr_block->allocated);
            printf("curr_block->block_size:%d\n",curr_block->block_size);
            printf("(alloc while loop)j+block_size:%d\n",j+block_size);
            if (!(curr_block->allocated) && j+block_size<curr_block->block_size){
                int old_size = curr_block->block_size;
                curr_block->block_size = block_size + j;
                curr_block->allocated = 1;
                block_info_t *new_block = ADD_BYTES(curr_block, curr_block->block_size);
                printf("(alloc found block to alloc)new_block:%p\n",new_block);
                printf("(alloc found block to alloc)curr_block->block_size:%d\n",curr_block->block_size);
                if ((uintptr_t)new_block%ALIGNMENT!=0){
                  int rem = (uintptr_t)new_block%ALIGNMENT;
                  new_block=ADD_BYTES(new_block,(ALIGNMENT-rem));
                }
                new_block->block_size= old_size - curr_block->block_size;
                printf("(alloc divide block to alloc)new_block->block_size:%d\n",new_block->block_size);
                new_block->allocated=0;
                return ADD_BYTES(curr_block, sizeof(block_info_t));
            }
            printf("(alloc not found block to alloc)curr_block:%p\n",curr_block);
            printf("curr_block->block_size:%d\n",curr_block->block_size);
            i+=curr_block->block_size;
            curr_block=ADD_BYTES(curr_block, curr_block->block_size);
            if ((uintptr_t)curr_block%ALIGNMENT!=0){
                int rem = (uintptr_t)curr_block%ALIGNMENT;
                curr_block=ADD_BYTES(curr_block,(ALIGNMENT-rem));
            }
            printf("(alloc check next block to alloc)curr_block:%p\n",curr_block);
        }
    return FAILURE;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void hl_release(void *heap, void *block) {
    mutex_lock(&malloc_lock);
    if (block==NULL){
        mutex_unlock(&malloc_lock);
        return;
    }
    printf("enter release\n");
    fflush(NULL);
    block = ADD_BYTES(block, -sizeof(block_info_t));
    heap_header_t *header = (heap_header_t *)heap;
    block_info_t *main_block=(block_info_t *)block;
    block_info_t* finder = find_block(header,main_block,main_block->block_size);
    if (finder!=NULL) {
        printf("(release block to release) finder:%p\n",finder);
        printf("(release block to release) finder->allocated:%d\n",finder->allocated);
        finder->allocated=0;
        block_info_t *next_block = ADD_BYTES(finder , finder->block_size);
        if ((uintptr_t)next_block%ALIGNMENT!=0){
            int rem = (uintptr_t)next_block%ALIGNMENT;
            next_block=ADD_BYTES(next_block,(ALIGNMENT-rem));
        }
        next_block=find_block(header, next_block, next_block->block_size);
        printf("(release find next block)next_block:%p\n",next_block);
        printf("next_block->block_size:%d\n",next_block->block_size);
        printf("finder->block_size:%d\n",finder->block_size);
        if (next_block!=NULL && next_block->allocated==0){
            int new_size=finder->block_size+next_block->block_size;
            next_block->block_size=0;
            next_block->allocated=0;
            next_block=NULL;
            finder->block_size=new_size;
            printf("(release new size of coalesced) new_size:%d\n",new_size);
    }
    }
    printf("(end of release block to release) finder:%p\n",finder);
    printf("(end of release block to release) finder->allocated:%d\n",finder->allocated);
    printf("(end of release block to release) finder->block_size:%d\n",finder->block_size);
    mutex_unlock(&malloc_lock);
}

void hl_release2(void *heap, void *block) {
    printf("enter release\n");
    if (block==NULL){
        return;
    }
    block = ADD_BYTES(block, -sizeof(block_info_t));
    heap_header_t *header = (heap_header_t *)heap;
    block_info_t *main_block=(block_info_t *)block;
    block_info_t* finder = find_block(header,main_block,main_block->block_size);
    if (finder!=NULL) {
        printf("(release block to release) finder:%p\n",finder);
        printf("(release block to release) finder->allocated:%d\n",finder->allocated);
        finder->allocated=0;
        block_info_t *next_block = ADD_BYTES(finder , finder->block_size);
        if ((uintptr_t)next_block%ALIGNMENT!=0){
            int rem = (uintptr_t)next_block%ALIGNMENT;
            next_block=ADD_BYTES(next_block,(ALIGNMENT-rem));
        }
        next_block=find_block(header, next_block, next_block->block_size);
        printf("(release find next block)next_block:%p\n",next_block);
        printf("next_block->block_size:%d\n",next_block->block_size);
        printf("finder->block_size:%d\n",finder->block_size);
        if (next_block!=NULL && next_block->allocated==0){
            int new_size=finder->block_size+next_block->block_size;
            next_block->block_size=0;
            next_block->allocated=0;
            next_block=NULL;
            finder->block_size=new_size;
            printf("(release new size of coalesced) new_size:%d\n",new_size);
    }
    }
    printf("(end of release block to release) finder:%p\n",finder);
    printf("(end of release block to release) finder->allocated:%d\n",finder->allocated);
    printf("(end of release block to release) finder->block_size:%d\n",finder->block_size);
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {
    mutex_lock(&malloc_lock);
    printf("enter resize\n");
     if (new_size==0){
         mutex_unlock(&malloc_lock);
        return FAILURE;
    }
    if (block==0){
        mutex_unlock(&malloc_lock);
        return hl_alloc(heap,new_size);
    }
    block = ADD_BYTES(block, -sizeof(block_info_t));
    heap_header_t *header = (heap_header_t *)heap;
    block_info_t *main_block=(block_info_t *)block;
    block_info_t* finder = find_block(header,main_block,main_block->block_size);
    printf("(resize block to resize) finder:%p\n",finder);
    printf("(resize block to resize) finder->allocated:%d\n",finder->allocated);
    printf("(resize block to resize) finder->block_size:%d\n",finder->block_size);
    new_size=new_size+sizeof(block_info_t);
    if (finder->block_size>=new_size){
        finder->block_size = new_size;
        mutex_unlock(&malloc_lock);
        return ADD_BYTES(finder, sizeof(block_info_t));;
    }else{
        block_info_t* new_block=hl_alloc2(heap, new_size);
        block_info_t* nnew_block = ADD_BYTES(new_block, -sizeof(block_info_t));
        printf("(resize block to alloc coz didn't fit) new_block:%p\n",nnew_block);
        printf("(resize block to alloc coz didn't fit) new_block->block_size:%d\n",nnew_block->block_size);
        if (nnew_block!=NULL){
            nnew_block->allocated=1;
            nnew_block->block_size=new_size;
            //memmove(ADD_BYTES(new_block,sizeof(block_info_t)),ADD_BYTES(finder,sizeof(block_info_t)), sizeof(char)*new_size);
            memmove(new_block,ADD_BYTES(finder,sizeof(block_info_t)), sizeof(char)*new_size);
            hl_release2(heap,finder);
            mutex_unlock(&malloc_lock); 
            return new_block; 
        }
    }
    mutex_unlock(&malloc_lock);
    return FAILURE;
}