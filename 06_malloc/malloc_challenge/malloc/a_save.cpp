//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Interfaces to get memory pages from OS
//

void* mmap_from_system(size_t size);
void munmap_to_system(void* ptr, size_t size);

//
// Struct definitions
//

typedef struct my_metadata_t {
    size_t size;
    struct my_metadata_t* left;
    struct my_metadata_t* right;
    //bool is_sequence;
} my_metadata_t;

typedef struct my_heap_t {
    my_metadata_t* free_head;
} my_heap_t;

void insertNode(my_metadata_t* metadata, my_metadata_t* target)
{
    //my_metadata_t* prev = NULL;
    assert(metadata);
    if(!target){
        return;
    }
    //target->is_sequence = false;
    while(true){
        /*if(target->size == metadata->size && metadata->is_sequence){
            assert(!target->left && !target->right);
            if(prev->left == metadata){
                prev->left = target;
            }else if(prev->right == metadata){
                prev->right = target;
            }else{
                assert(false);
            }
            target->is_sequence = true;
            target->left = metadata->left;
            target->right = metadata;
            metadata->left = NULL;
            return;
        }*/
        if (target->size < metadata->size) {
            if (!metadata->left) {
                metadata->left = target;
                //target->is_sequence = true;
                return;
            } else {
                //prev = metadata;
                metadata = metadata->left;
            }
        } else {
            if (!metadata->right) {
                metadata->right = target;
                //target->is_sequence = true;
                return;
            } else {
                //prev = metadata;
                metadata = metadata->right;
            }
        }
    }
}


//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap;

//
// Helper functions (feel free to add/remove/edit!)
//

void my_add_to_free_list(my_metadata_t* metadata)
{
    assert(metadata);
    if(!my_heap.free_head){
        //printf("new head in my_add_to_free_list \n");
        my_heap.free_head = metadata;
    } else {
        //printf("calling insertNode int my_add_to_free_list \n");
        insertNode(my_heap.free_head, metadata);
    }
}

void my_remove_from_free_list(my_metadata_t* metadata, my_metadata_t* prev)
{
    assert(metadata);
    //printf("my_remove_from_free_list \n");
    if (prev) {
        if(metadata->right){
            if (metadata->size < prev->size) {
                prev->left = metadata->right;
            } else {
                prev->right = metadata->right;
            }
            //printf("calling insertNode int my_remove_from_free_list with prev \n");
            insertNode(metadata->right, metadata->left);
        } else {
            if (metadata->size < prev->size) {
                prev->left = metadata->left;
            } else {
                prev->right = metadata->left;
            }
        }
    } else {
        if(metadata->right){
            my_heap.free_head = metadata->right;
            //printf("calling insertNode int my_remove_from_free_list \n");
            insertNode(metadata->right, metadata->left);
        }else{
            my_heap.free_head = metadata->left;
        }
    }
}

//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
void my_initialize()
{
    my_heap.free_head = NULL;
}

// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().
void* my_malloc(size_t size)
{
    my_metadata_t* metadata = my_heap.free_head;
    my_metadata_t* prev = NULL;
    my_metadata_t* best_fit = NULL;
    my_metadata_t* best_fit_prev = NULL;
    // First-fit: Find the first free slot the object fits.
    // TODO: Update this logic to Best-fit!
    //size_t searching_count = 0;
    while (metadata) {
        if (metadata->size == size) {
            best_fit = metadata;
            best_fit_prev = prev;
            break;
        }
        if (metadata->size < size) {
            prev = metadata;
            metadata = metadata->right;
        } else {
            best_fit = metadata;
            best_fit_prev = prev;
            prev = metadata;
            metadata = metadata->left;
        }
    }
    // now, metadata points to the first free slot
    // and prev is the previous entry.

    //printf("%p\n",best_fit);
    //printf("%p\n",best_fit_prev);

    if (!best_fit) {
        //printf("called new memory rigion\n");
        // There was no free slot available. We need to request a new memory region
        // from the system by calling mmap_from_system().
        //
        //     | metadata | free slot |
        //     ^
        //     metadata
        //     <---------------------->
        //            buffer_size
        size_t buffer_size = 4096;
        my_metadata_t* best_fit = (my_metadata_t*)mmap_from_system(buffer_size);
        best_fit->size = buffer_size - sizeof(my_metadata_t);
        //TODO
        best_fit->left = NULL;
        best_fit->right = NULL;

        // Add the memory region to the free list.
        //printf("%ld\n", best_fit->size);
        my_add_to_free_list(best_fit);
        // Now, try my_malloc() again. This should succeed.
        return my_malloc(size);
    }

    // |ptr| is the beginning of the allocated object.
    //
    // ... | metadata | object | ...
    //     ^          ^
    //     metadata   ptr
    //printf("called best fit\n");
    void* ptr = best_fit + 1;
    size_t remaining_size = best_fit->size - size;
    best_fit->size = size;
    // Remove the free slot from the free list.
    my_remove_from_free_list(best_fit, best_fit_prev);

    if (remaining_size > sizeof(my_metadata_t)) {
        //printf("create new metadata for remaining free slot\n");
        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | object | metadata | free slot | ...
        //     ^          ^        ^
        //     metadata   ptr      new_metadata
        //                 <------><---------------------->
        //                   size       remaining size
        my_metadata_t* new_metadata = (my_metadata_t*)((char*)ptr + size);
        new_metadata->size = remaining_size - sizeof(my_metadata_t);
        new_metadata->left = NULL;
        new_metadata->right = NULL;
        // Add the remaining free slot to the free list.
        my_add_to_free_list(new_metadata);
    }
    return ptr;
}

// This is called every time an object is freed.  You are not allowed to
// use any library functions other than mmap_from_system / munmap_to_system.
void my_free(void* ptr)
{
    // Look up the metadata. The metadata is placed just prior to the object.
    //
    // ... | metadata | object | ...
    //     ^          ^
    //     metadata   ptr
    my_metadata_t* metadata = (my_metadata_t*)ptr - 1;
    //assert(!metadata->left && !metadata->right);
    metadata->right = NULL;
    metadata->left = NULL;
    // Add the free slot to the free list.
    my_add_to_free_list(metadata);
}

// This is called at the end of each challenge.
void my_finalize()
{
    // Nothing is here for now.
    // feel free to add something if you want!
}

void test()
{
    // Implement here!
    assert(1 == 1); /* 1 is 1. That's always true! (You can remove this.) */
}
