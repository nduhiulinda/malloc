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



/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
int hl_init(void *heap, unsigned int heap_size) {
    
    if (heap_size < MIN_HEAP_SIZE){
        return FAILURE;
    }
    //ensure heap is 8-byte aligned
    if (heap%ALIGNMENT!=0){
        int rem = heap%ALIGNMENT;
        heap=heap+rem;
    }
    //initialize heap metadata
    heap_header_t *header = (heap_header_t *)heap;
    header->heap_size = heap_size;
    //initialize first block metadata
    block_info_t *block_head = ADD_BYTES(header, sizeof(heap_header_t));
    block_head->size = heap_size-(sizeof(int)+sizeof(_block_info_t));
    if (block_head->size%ALIGNMENT!=0){
        int rem = heap%ALIGNMENT;
        block_head->size=block_head->size+rem;
    }
    block_head->allocated=0;

    return SUCCESS;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_alloc(void *heap, unsigned int block_size) {

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

