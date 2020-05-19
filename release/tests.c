#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"
#include <pthread.h>

#define HEAP_SIZE 1024
#define NUM_TESTS 24
#define NPOINTERS 100

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

// TODO: Add test descriptions as you add more tests...
const char* test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "single init, should fail if heap not aligned",
    /* 5  */ "single init then single alloc, should pass if block is aligned",
    /* 6  */ "alloc without init, should pass if alloc returns NULL",
    /* 7  */ "your description here",
    /* 8  */ "your description here",
    /* 9  */ "your description here",
    /* 10 */ "your description here",
    /* 11 */ "your description here",
    /* 12 */ "your description here",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "example threading test",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "your description here",
    /* 18 */ "your description here",
    /* 19 */ "your description here",
    /* 20 */ "your description here",
    /* 21 */ "your description here",
    /* 22 */ "your description here",
    /* 23 */ "your description here",
};



/* ------------------ COMPLETED SPEC TESTS ------------------------- */

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init
 * SPECIFICATION BEING TESTED:
 * hl_init should successfully complete (without producing a seg
 * fault) for a HEAP_SIZE of 1024 or more.
 *
 * MANIFESTATION OF ERROR:
 * A basic test of hl_init.  If hl_init has an eggregious programming
 * error, this simple call would detect the problem for you by
 * crashing.
 *
 * Note: this shows you how to create a test that should succeed.
 */
int test00() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    return SUCCESS;
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is room in the heap for a request for a block, hl_alloc
 * should sucessfully return a pointer to the requested block.
 *
 * MANIFESTATION OF ERROR:
 * The call to hl_alloc will return NULL if the library cannot find a
 * block for the user (even though the test is set up such that the
 * library should be able to succeed).
 */
int test01() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns null, test01 returns FAILURE (==0)
    return (hl_alloc(heap, HEAP_SIZE/2) != NULL);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is not enough room in the heap for a request for a block,
 * hl_alloc should return NULL.
 *
 * MANIFESTATION OF ERROR:
 * This test is designed to request a block that is definitely too big
 * for the library to find. If hl_alloc returns a pointer, the library is flawed.
 *
 * Notice that heaplame's hl_alloc does NOT return NULL. (This is one
 * of many bugs in heaplame.)
 *
 * Note: this shows you how to create a test that should fail.
 *
 * Surely it would be a good idea to test this SPEC with more than
 * just 1 call to alloc, no?
 */
int test02() {

    // simulated heap is just an array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns NULL, test02 returns SUCCESS (==1)
    return !hl_alloc(heap, HEAP_SIZE*2);

}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * There should be no hard-coded limit to the number of blocks heaplib
 * can support. So if the heap gets larger, so should the number of
 * supported blocks.
 *
 * MANIFESTATION OF ERROR:
 * See how many blocks are supported with heap size N. This number should
 * increase with heap size 2N. Otherwise fail!
 *
 * Note: unless it is fundamentally changed, heaplame will always fail
 * this test. That is fine. The test will have more meaning when run on
 * your heaplib.c
 */
int test03() {

    // now we simulate 2 heaps, once 2x size of the first
    char heap[HEAP_SIZE], heap2[HEAP_SIZE*2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while(true) {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE*2);

    while(true) {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }
#ifdef PRINT_DEBUG
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:hl_init
 * SPECIFICATION BEING TESTED:
 * alignment of heap
 *
 *
 * MANIFESTATION OF ERROR:
 * Failure if heap is not 8-byte aligned and first block is not 8-byte aligned
 * 
 *
 */
int test04() {
    char heap[HEAP_SIZE];
    int suc = hl_init((void*)heap+7, HEAP_SIZE);
    heap_header_t *header = (heap_header_t *)heap;
    if ( suc && ((uintptr_t)header%ALIGNMENT==0) && ((uintptr_t)header->blocks[0].block_size%ALIGNMENT==0)){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:hl_alloc
 * SPECIFICATION BEING TESTED:
 * block_size that isn't 8-byte aligned
 * 
 * MANIFESTATION OF ERROR:
 * Error if block is not 8-byte aligned
 *
 */
int test05() {
 
    char heap[HEAP_SIZE+4];
 
    hl_init((void*)heap+4, HEAP_SIZE);
 
    int *array = hl_alloc(heap, 8);
    if ((uintptr_t) array%8==0){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_alloc
 * SPECIFICATION BEING TESTED: check if heap was initialized
 *
 *
 * MANIFESTATION OF ERROR: 
 * If heap was not initialized, return NULL(0)
 *
 */
int test06() {
    char heap[HEAP_SIZE];
  
    if (hl_alloc(heap, 8)==0){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_release & hl_alloc
 * SPECIFICATION BEING TESTED: frees allocated block
 *
 *
 * MANIFESTATION OF ERROR:
 * Fails if block is not released
 *
 */
int test07() {
//     char heap[HEAP_SIZE];
//     hl_init(heap, HEAP_SIZE);     
//     int *block = hl_alloc(heap, 8);
//     if ((int)hl_release(heap, block)==0){
//         return FAILURE;
//     }
    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_release & hl_alloc
 * SPECIFICATION BEING TESTED: if block is zero, acts as NOP
 *
 * MANIFESTATION OF ERROR:
 */
int test08() {
//     char heap[HEAP_SIZE];
//     hl_init(heap, HEAP_SIZE);     
//     int *block = hl_alloc(heap, 8);
//     heap_header_t *header = (heap_header_t *)heap;
//     int block_no = find_block(heap_header_t *header, void *block);
//     hl_release(heap, 0);
//     if (find_block(heap_header_t *header, void *block)!=block_no){
//         return FAILURE;
//     }
    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: content of block should be preserved after resize
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test09() {
//     char heap[HEAP_SIZE];
//     hl_init(heap, HEAP_SIZE);     
//     int *block = hl_alloc(heap, 8);
//     heap_header_t *header = (heap_header_t *)heap;
//     void *hl_resize(heap, block, 16);
//     int block_no = find_block(heap_header_t *header, void *block);
//     header->blocks[block_no] 

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: if block is NULL, allocate block
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test10() {
//     char heap1[HEAP_SIZE];
//     char heap2[HEAP_SIZE];
//     hl_init(heap1, HEAP_SIZE);   
//     hl_init(heap2, HEAP_SIZE);    
//     int *block = hl_alloc(heap1, 8); 
//     int *block2 = NULL; 
//     void *hl_resize(heap2, block2, 8); 
//     heap_header_t *header1 = (heap_header_t *)heap1;
//     heap_header_t *header2 = (heap_header_t *)heap2; 
//     int count=0;

//     for (int i = 0; i < 8; i++){
//         if ((header1->blocks[i].block==header2->blocks[i].block) && header1->in_use_f[i]==header2->in_use_f[i]){
//             count++;
//         }
//         else{
//             break;
//         }   
//     }
     
//     return (count==8);
return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test11() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test12() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test13() {

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test14() {

    return FAILURE;
}

typedef struct {
    int arg1;
    void* heap;
    pthread_barrier_t* barrier;
} arg_struct;

/* The signature for a function run as a thread is a single void* argument returning a void*.
 * If you want to pass in arguments, you need to define a struct of arguments and cast manually.
 */

void* thread_function(void* ptr){
    arg_struct* args = (arg_struct*) ptr;
    pthread_barrier_t* barrier = args->barrier;
    //wait for all threads to synchronize at a barrier so they will run concurrently
    pthread_barrier_wait(barrier);
    hl_alloc(args->heap, 16);
    //do stuff with your arguments here
    return NULL;
}


/* THIS TEST IS NOT FINISHED.  It is a scaffold you can use for a threaded test.
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * Malloc library must be thread-safe: multiple threads simultaneously using the library
 * should not interfere with each other or cause a deadlock
 *
 * MANIFESTATION OF ERROR:
 * Test runs forever on a deadlock, or integrity is violated on a race condition.
 */
int test15() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    int n_threads = 100;
    //int num_iter = 10000;
    pthread_t threads[n_threads];
    int irets[n_threads]; //return values of the threads

    //initialize a synchronization barrier
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, n_threads);

    arg_struct args[n_threads];
    for(int i = 0; i < n_threads; i++){
	args[i] = (arg_struct) {.arg1=1, .barrier = &barrier, .heap=&heap}; //inline initialization of a struct

    }
    

    
    //create threads to execute your function
    for(int i = 0; i < n_threads; i++){
	irets[i] = pthread_create(&threads[i], NULL, thread_function, (void*) &args[i]);
    }
    //wait for all of the threads to finish before moving on
    for(int i = 0; i < n_threads; i++){
	pthread_join(threads[i], NULL);
    }
    //cleanup the barrier
    pthread_barrier_destroy(&barrier);
    printf("Thread 0 returned: %d\n", irets[0]);
    
    return FAILURE;
}

/* ------------------ STRESS TESTS ------------------------- */

/* THIS TEST IS NOT FINISHED. It is a stress test, but it does not
 * actually test to see whether the library or the user writes
 * past the heap. You are encouraged to complete this test.
 *
 * FUNCTIONS BEING TESTED: alloc, free
 * SPECIFICATION BEING TESTED:
 * The library should not give user "permission" to write off the end
 * of the heap. Nor should the library ever write off the end of the heap.
 *
 * MANIFESTATION OF ERROR:
 * If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test16() {
    int n_tries    = 10000;
    int block_size = 16;

    // 1024 bytes of padding
    // --------------------
    // 1024 bytes of "heap"
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"
    char memarea[HEAP_SIZE*3];

    char *heap_start = &memarea[1024]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];

    // zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            pointers[index] = hl_alloc(heap_start,  block_size);
        }
        else{
            hl_release(heap_start, pointers[index]);
            pointers[index] = 0;
        }
    }
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test17() {

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test18() {

    return FAILURE;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test19() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test20() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test21() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test22() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test23() {

    return FAILURE;
}
