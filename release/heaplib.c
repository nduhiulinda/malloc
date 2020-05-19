#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "spinlock.h"

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
    block_info_t blocks[1];
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

	printf("block[%d] =[%d,%d]\n", 0, header->blocks[0].block_size, header->blocks[0].allocated);

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
    // header->blocks[0]->block_size = heap_size-(sizeof(heap_header_t));
    block_info_t *block_head = ADD_BYTES(heap, sizeof(heap_header_t));
    header->blocks[0] = *block_head;
    header->blocks[0].block_size = heap_size-(sizeof(heap_header_t));
    if ((uintptr_t)header->blocks[0].block_size%ALIGNMENT!=0){
        int rem = (uintptr_t)header->blocks[0].block_size%ALIGNMENT;
        header->blocks[0].block_size=header->blocks[0].block_size-rem;
    }
    header->blocks[0].allocated=0;
    print_debug_heap_header(header);

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
void *hl_alloc(void *heap, unsigned int block_size) {
    heap_header_t *header = (heap_header_t *)heap;
    // void *next_free_byte = ADD_BYTES(header,sizeof(heap_header_t));
    int i = sizeof(heap_header_t);
    int j = sizeof(block_info_t);
    // block_info_t *first_block = (block_info_t *)header->blocks[0];
    // if !(first_block->allocated){
    //     if (i+block_size+j<header->heap_size && j+block_size<first_block->block_size){
    //         int old_size = first_block->block_size;
    //         first_block->block_size = block_size + j;
    //         if ((uintptr_t)first_block->block_size%ALIGNMENT!=0){
    //             int rem = (uintptr_t)first_block->block_size%ALIGNMENT;
    //             first_block->block_size=first_block->block_size-rem;
    //         }
    //         first_block->allocated = 1;
    //         block_info_t *new_block = &first_block + first_block->block_size;
    //         new_block->block_size= old_size - first_block->block_size;
    //         new_block->allocated=0;
    //         print_debug_alloc(first_block);
    //         return first_block;
    //     }
    // } else{
        block_info_t *curr_block = &header->blocks[0];
        while (i+block_size+j<header->heap_size){

            if (!(curr_block->allocated) && j+block_size<curr_block->block_size){
                int old_size = curr_block->block_size;
                curr_block->block_size = block_size + j;
                if ((uintptr_t)curr_block->block_size%ALIGNMENT!=0){
                    int rem = (uintptr_t)curr_block->block_size%ALIGNMENT;
                    curr_block->block_size=curr_block->block_size-rem;
                }
                curr_block->allocated = 1;
                block_info_t *new_block = curr_block + curr_block->block_size;
                new_block->block_size= old_size - curr_block->block_size;
                new_block->allocated=0;
                print_debug_alloc(curr_block);
                print_debug_block_header(curr_block);
                print_debug_heap_header(header);
                return curr_block;
            }
            i+=curr_block->block_size;
            curr_block=curr_block + curr_block->block_size;
        }
    // }
		

	return FAILURE;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void hl_release(void *heap, void *block) {

}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *block, unsigned int new_size) {

    return FAILURE;
}

