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
    struct my_metadata_t* before;
    struct my_metadata_t* after;
    bool is_free;
} my_metadata_t;

typedef struct my_heap_t {
    my_metadata_t* free_head;
} my_heap_t;

//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap;

//
// Helper functions (feel free to add/remove/edit!)
//


void my_insertNode(my_metadata_t* metadata, my_metadata_t* target)
{
    assert(metadata);
    my_metadata_t* node = metadata;
    if (!target) {
        return;
    }
    while (true) {
        if (target->size < node->size) {
            if (!node->left) {
                node->left = target;
                return;
            } else {
                node = node->left;
            }
        } else {
            if (!node->right) {
                node->right = target;
                return;
            } else {
                node = node->right;
            }
        }
    }
}


void my_add_to_free_list(my_metadata_t* metadata)
{
    assert(metadata);
    if (!my_heap.free_head) {
        my_heap.free_head = metadata;
    } else {
        my_insertNode(my_heap.free_head, metadata);
    }
}

void my_remove_from_free_list_with_prev(my_metadata_t* metadata, my_metadata_t* prev)
{
  assert(metadata);
    if (prev) {
        if(metadata->right){
            if (metadata->size < prev->size) {
                prev->left = metadata->right;
            } else {
                prev->right = metadata->right;
            }
            my_insertNode(metadata->right, metadata->left);
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
            my_insertNode(metadata->right, metadata->left);
        }else{
            my_heap.free_head = metadata->left;
        }
    }
}

bool my_remove_from_free_list(my_metadata_t* metadata){
  my_metadata_t* node = my_heap.free_head;
  my_metadata_t* prev = NULL;
  assert(metadata);
  while(node){
    if(metadata == node){
      my_remove_from_free_list_with_prev(metadata, prev);
      return true;
    }else if(metadata->size < node->size){
      prev = node;
      node = node->left;
    }else{
      prev = node;
      node = node->right;
    }
  }
  assert(false);
  return false;
}

void my_fill_gap(my_metadata_t* metadata){
  if(!metadata->after){
    return;
  }
  void* ptr_at_end = (metadata + 1);
  void* ptr_in_front = (metadata->after - 1);
  metadata->size += ((size_t)ptr_in_front + metadata->size - 1 - (size_t)ptr_at_end);
}

void my_merge_left(my_metadata_t* metadata)
{
  if(!metadata->after || !metadata->after->is_free){
    return;
  }

  my_remove_from_free_list(metadata->after);
  
  metadata->size += (sizeof(my_metadata_t) + metadata->after->size);
  
  metadata->after = metadata->after->after;
  if(metadata->after){
    metadata->after->before = metadata;
  }

}

void my_merge_right(my_metadata_t* metadata){

  if(metadata->before && metadata->before->is_free){

  my_remove_from_free_list(metadata->before);

  metadata = metadata->before;

  metadata->left = NULL;
  metadata->right = NULL;

  metadata->size += (sizeof(my_metadata_t) + metadata->after->size);
  
  metadata->after = metadata->after->after;
  if(metadata->after){
    metadata->after->before = metadata;
  }
  }
  my_add_to_free_list(metadata);
  /*metadata->before->size += (sizeof(my_metadata_t) + metadata->size);
  
  metadata->before->after = metadata->after;
  if(metadata->after){
    metadata->after->before = metadata->before;
  }

  metadata = metadata->before;*/
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

        best_fit->left = NULL;
        best_fit->right = NULL;

        best_fit->before = NULL;
        best_fit->after = NULL;
        
        best_fit->is_free = true;
        // Add the memory region to the free list.
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
    best_fit->is_free = false;
    my_remove_from_free_list_with_prev(best_fit, best_fit_prev);
    // Remove the free slot from the free list.

    if (remaining_size > sizeof(my_metadata_t)) {
        //printf("create new metadata for remaining free slot\n");
        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | object | metadata | free slot | ...
        //     ^          ^        ^
        //     metadata   ptr      new_metadata
        //                 <------><---------------------->
        //                   size       remaining size
        best_fit->size = size;
        my_metadata_t* new_metadata = (my_metadata_t*)((char*)ptr + size);
        new_metadata->size = remaining_size - sizeof(my_metadata_t);
        new_metadata->left = NULL;
        new_metadata->right = NULL;
        new_metadata->is_free = true;

        // Add the remaining free slot to the free list.
        my_add_to_free_list(new_metadata);
        
        /*before after*/
        assert(best_fit);
        new_metadata->after = best_fit->after;
        new_metadata->before = best_fit;
        if(best_fit->after){
          best_fit->after->before = new_metadata;
        }
        best_fit->after = new_metadata;
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
    metadata->is_free = true;
    // Add the free slot to the free list.
    //my_fill_gap(metadata);
    my_merge_left(metadata);
    my_merge_right(metadata);
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
