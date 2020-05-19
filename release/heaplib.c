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
 */
void *hl_alloc(void *heap, unsigned int block_size) {
    // heap_header_t *header = (heap_header_t *)heap;
    // void *next_free_byte = ADD_BYTES(header,sizeof(heap_header_t));
    // int i=sizeof(heap_header_t);
    // 	while ( i<heap->heap_size) {
	// 	if (header->blocks[i].allocated==0) {      
	// 		header->blocks[i].allocated = 1;  
	// 		header->blocks[i].block_size = block_size; 
	// 		break;
	// 	} else {
	// 		next_free_byte = header->blocks[i].block + header->blocks[i].block_size;
	// 	}
    //     i+=header->blocks[i].block_size;
	// }
	// if (i == header->heap_size)
		return FAILURE;

    //print_debug_alloc(next_free_byte);
    //print_debug_heap_header(header);

    // return next_free_byte;
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

