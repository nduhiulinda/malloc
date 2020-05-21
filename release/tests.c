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

// TODO: Add test descriptions as you add more tests...
const char* test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "single init then single alloc, should pass if block is aligned",
    /* 5  */ "single init then single alloc of size 0, should fail if block is not NULL",
    /* 6  */ "single init then single alloc, should fail if alloc doesn't return NULL when full",
    /* 7  */ "alloc the rlease then alloc on same block, should fail if block is not released/can't be reallocated",
    /* 8  */ "if block is zero, release acts as NOP",
    /* 9  */ "after resizing a block, the contents of the block are preserved",
    /* 10 */ "if block is NULL, resize acts as allocate",
    /* 11 */ "single init then single alloc, should pass if block is aligned and heap is not",
    /* 12 */ "if new_size is 0, resize returns NULL",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "multiple threads simultaneously using the library should not interfere with each other or cause a deadlock",
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
 * FUNCTIONS BEING TESTED:hl_init and hl_alloc
 * SPECIFICATION BEING TESTED:
 * alignment of block even if heap isn't aligned
 *
 *
 * MANIFESTATION OF ERROR:
 * Failure if block is not 8-byte aligned 
 * 
 *
 */
int test04() {
    char heap[HEAP_SIZE];

    int *heap_ptr = (int*)heap;
    uintptr_t addr = (uintptr_t) heap_ptr;    

    if (addr % 8 == 0){
        heap_ptr = (int*)ADD_BYTES(heap_ptr, 1);
        }

    hl_init(heap_ptr, HEAP_SIZE);
    uintptr_t block = (uintptr_t) hl_alloc(heap_ptr, 13);
    if (block%ALIGNMENT==0){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:hl_alloc
 * SPECIFICATION BEING TESTED: alloc block of size zero
 * 
 * MANIFESTATION OF ERROR:
 * Failure if block of size zero is not NULL
 *
 */
int test05() {
    return FAILURE;

}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_alloc
 * SPECIFICATION BEING TESTED: return NULL if can't alloc
 *
 *
 * MANIFESTATION OF ERROR: 
 * Fail if alloc doesn't return NULL when full
 *
 */
int test06() {
    // char heap[HEAP_SIZE];
    // hl_init(heap, HEAP_SIZE); 
    // hl_alloc(heap, 8);
    // hl_alloc(heap, 64);  
    // hl_alloc(heap, 512); 
    // int * block = hl_alloc(heap, 512);
    // if (block==0){
    //     return SUCCESS;
    // }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_release & hl_alloc
 * SPECIFICATION BEING TESTED: frees allocated block and reallocates block
 *
 *
 * MANIFESTATION OF ERROR:
 * Fails if block is not released/ can't reallocated
 *
 */
int test07() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);     
    hl_alloc(heap, 8);
    int *block3 = hl_alloc(heap, 10);
    hl_release(heap, block3);
    int *block2 = hl_alloc(heap, 8);
    if (block3==block2){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_release & hl_alloc
 * SPECIFICATION BEING TESTED: if block is NULL, release acts as NOP
 *
 * MANIFESTATION OF ERROR:
 */
int test08() {
    char heap[HEAP_SIZE];
    char heap2[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE); 
    hl_alloc(heap, 8);
    hl_alloc(heap, 64);
    memcpy(heap2,heap,HEAP_SIZE);
    hl_release(heap, NULL);
    if (memcmp(heap,heap2,HEAP_SIZE)==0){
        return SUCCESS;
    }
    return FAILURE;

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
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);     
    int *block = hl_alloc(heap, 8);
    memset(block,'a',8);
    int *resized=hl_resize(heap, block, 16);
    int *resized_val;
    int same = 1;
    for (int i=0; i<8; i++){
    #ifdef PRINT_DEBUG
    printf("block starts at addr %p\n"   // C printing trick.
		   "block_size = %d\n",            // Notice: no commas between lines
		   resized, 16); 

	printf("resized[%d] = %p\n", i, (int*)ADD_BYTES(resized, i));
    printf(" size of resized[%d] =%ldu\n", i, sizeof(char));

    #endif
        resized_val = ADD_BYTES(resized, i);
        char val = *resized_val;
        #ifdef PRINT_DEBUG
            printf("val = %c\n", val);
        #endif
        if (val!='a'){
            same = 0;
        }
    }
    if (same){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: if block is NULL, resize acts as allocate
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test10() {
    char heap2[HEAP_SIZE];  
    hl_init(heap2, HEAP_SIZE);    
    int *block2 = NULL; 
    int *resize_block=hl_resize(heap2, block2, 8); 
    if (resize_block!=NULL){
        return SUCCESS;
    }

    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:hl_init and hl_alloc
 * SPECIFICATION BEING TESTED:
 * alignment of block  if heap is aligned
 *
 *
 * MANIFESTATION OF ERROR:
 * Failure if block is  not 8-byte aligned 
 * 
 *
 */
int test11() {
    char heap[HEAP_SIZE];
    int align=0;
    int *heap_ptr = (int*)heap;
    if ((uintptr_t)heap_ptr % 8 == 0){
        align=1;
        }

    hl_init(heap_ptr, HEAP_SIZE);
    uintptr_t block = (uintptr_t) hl_alloc(heap_ptr, 13);
    if (block%ALIGNMENT==0 &&  align){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:hl_init, hl_resize.
 * SPECIFICATION BEING TESTED: if new_size is 0, resize returns NULL.
 *
 *
 * MANIFESTATION OF ERROR: Fail if new_size = 0 does not return NULL.
 *
 */
int test12() {
    // char heap1[HEAP_SIZE];
    // hl_init(heap1, HEAP_SIZE);  
    // int * block = hl_alloc(heap1, 56);    
    // int *resize_block=hl_resize(heap1, block, 0); 
    // if (resize_block==NULL){
    //     return FAILURE;
    // }
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
    char heap[HEAP_SIZE];  
    hl_init(heap, HEAP_SIZE);    
    int *block1 = hl_alloc(heap, 20); 
    printf("1\n");
    fflush(NULL);
    //int *block2 = hl_alloc(heap, 20);
    //int *block3 = hl_alloc(heap, 400);
    hl_release(heap,block1);
    printf("2\n");
    fflush(NULL);
    // hl_release(heap,block2);
    // hl_release(heap,block3);
    hl_alloc(heap, 20);
    printf("3\n");
    fflush(NULL);
    // int *block6 = hl_alloc(heap, 20);
    // int *block7 = hl_alloc(heap, 400);
    // if (block7==block3 && block6==block2 && block5==block1){
    //     return SUCCESS;
    // }
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
    char heap[HEAP_SIZE];  
    hl_init(heap, HEAP_SIZE);    
    int *block1 = hl_alloc(heap, 256); 
    int *block2 = hl_alloc(heap, 480);
    int *block3 = hl_alloc(heap, 600);
    hl_release(heap,block3);
    hl_release(heap,block2);
    hl_release(heap,block1);
    hl_alloc(heap, 256);
    hl_alloc(heap, 480);
    int *block7 = hl_alloc(heap, 600);
    if (block7==NULL && block3==NULL ){
        return SUCCESS;
    }
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
