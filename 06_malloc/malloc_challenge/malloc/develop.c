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
#include <sys/mman.h>

//
// Interfaces to get memory pages from OS
//

void* mmap_from_system(size_t size);
void munmap_to_system(void* ptr, size_t size);

//
// Struct definitions
//

typedef struct my_is_small_t {
    bool is_small;
    bool is_cache;
} my_is_small_t;

typedef struct my_metadata_t {
    size_t size;
    struct my_metadata_t* left;
    struct my_metadata_t* right;
    struct my_metadata_t* before;
    struct my_metadata_t* after;
    bool is_free;
} my_metadata_t;

typedef struct my_metadata_small_t {
    size_t size;
    struct my_metadata_small_t* next;
} my_metadata_small_t;

typedef struct my_cache_t{
    struct my_cache_t* next;
} my_cache_t;

typedef struct my_cache_pool_t{
    size_t size;
} my_cache_pool_t;

typedef struct my_heap_t {
    my_metadata_t* free_head;
    my_metadata_small_t* free_small_head;
    my_cache_t* cache_head;
    my_cache_pool_t* cache_pool;
    size_t num_of_data;
    size_t average;
    //size_t variance;
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
        if (metadata->right) {
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
        if (metadata->right) {
            my_heap.free_head = metadata->right;
            my_insertNode(metadata->right, metadata->left);
        } else {
            my_heap.free_head = metadata->left;
        }
    }
}

bool my_remove_from_free_list(my_metadata_t* metadata)
{
    my_metadata_t* node = my_heap.free_head;
    my_metadata_t* prev = NULL;
    assert(metadata);
    while (node) {
        if (metadata == node) {
            my_remove_from_free_list_with_prev(metadata, prev);
            return true;
        } else if (metadata->size < node->size) {
            prev = node;
            node = node->left;
        } else {
            prev = node;
            node = node->right;
        }
    }
    assert(false);
    return false;
}

my_metadata_t* my_fill_gap(my_metadata_t* metadata)
{
    if(metadata->after){
        void* ptr_after = metadata->after;
        void* ptr_now = metadata + 1;
        metadata->size = ((char*)ptr_after - (char*)ptr_now - sizeof(my_is_small_t));
    }
    if(metadata->before){
        void* ptr_before = metadata->before + 1;
        void* ptr_now = metadata;
        char* ptr_for_new_metadata =((char*)ptr_before + sizeof(my_is_small_t) + metadata->before->size);
        if(ptr_for_new_metadata == (char*)ptr_now){
            return metadata;
        }
        assert(ptr_for_new_metadata < (char*)ptr_now);
        my_metadata_t* before = metadata->before;
        my_metadata_t* after = metadata->after;
        size_t size = metadata->size + (char*)ptr_now - ptr_for_new_metadata;
        metadata->after = NULL;
        metadata->before = NULL;
        metadata->is_free = false;
        metadata->size = 0;
        metadata->left = NULL;
        metadata->right = NULL;

        my_metadata_t* new_metadata = (my_metadata_t*)(ptr_for_new_metadata);
        my_is_small_t* is_small = (my_is_small_t*)(ptr_for_new_metadata + sizeof(my_metadata_t));

        new_metadata->size = size;

        new_metadata->before = before;
        new_metadata->after = after;

        before->after = new_metadata;
        if(after){
            after->before = new_metadata;
        }
        return new_metadata;
    }
    return metadata;
}

void my_merge_left(my_metadata_t* metadata)
{
    if (!metadata->after || !metadata->after->is_free) {
        return;
    }

    my_remove_from_free_list(metadata->after);

    metadata->size += (sizeof(my_metadata_t) + sizeof(my_is_small_t) + metadata->after->size);

    metadata->after = metadata->after->after;
    if (metadata->after) {
        metadata->after->before = metadata;
    }
}

void my_merge_right(my_metadata_t* metadata)
{

    if (metadata->before && metadata->before->is_free) {

        my_remove_from_free_list(metadata->before);

        metadata = metadata->before;

        metadata->left = NULL;
        metadata->right = NULL;

        metadata->size += (sizeof(my_metadata_t) + sizeof(my_is_small_t) + metadata->after->size);

        metadata->after = metadata->after->after;
        if (metadata->after) {
            metadata->after->before = metadata;
        }
    }
    my_add_to_free_list(metadata);
}

void my_add_to_free_list_small(my_metadata_small_t* metadata)
{
    assert(!metadata->next);
    metadata->next = my_heap.free_small_head;
    my_heap.free_small_head = metadata;
}

void my_remove_from_free_list_small(my_metadata_small_t* metadata, my_metadata_small_t* prev)
{
    if (prev) {
        prev->next = metadata->next;
    } else {
        my_heap.free_small_head = metadata->next;
    }
    metadata->next = NULL;
}

void my_merge_left_small(my_metadata_small_t* metadata)
{
    void* ptr_after_metadata = metadata + 1;
    my_metadata_small_t* next_metadata = (my_metadata_small_t*)((char*)ptr_after_metadata + sizeof(my_is_small_t) + metadata->size);

    my_metadata_small_t* node = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    while(node){
        if(node == next_metadata){
            my_remove_from_free_list_small(next_metadata, prev);
            metadata->size += next_metadata->size + sizeof(my_metadata_small_t) + sizeof(my_is_small_t);
            //next_metadata->next = NULL;
            //next_metadata->size = 0;
            return;
        }else{
            prev = node;
            node = node->next;
        }
    }
}

void my_merge_right_or_add_small(my_metadata_small_t* metadata)
{
    void* ptr_of_metadata = metadata;
    my_metadata_small_t* node = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    while(node){
        void* ptr_after_node = node + 1;
        if((char*)ptr_of_metadata == (char*)ptr_after_node + sizeof(my_is_small_t) + node->size){
            my_remove_from_free_list_small(node, prev);
            node->size += metadata->size + sizeof(my_metadata_small_t) + sizeof(my_is_small_t);
            my_add_to_free_list_small(node);
            //next_metadata->next = NULL;
            //next_metadata->size = 0;
            return;
        }else{
            prev = node;
            node = node->next;
        }
    }
    my_add_to_free_list_small(metadata);
}

void my_add_to_cache(my_cache_t* metadata)
{
    assert(!metadata->next);
    metadata->next = my_heap.cache_head;
    my_heap.cache_head = metadata;
}

//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
void my_initialize()
{
    printf("%i\n", (int)sizeof(my_is_small_t));
    printf("%i\n", (int)sizeof(my_metadata_small_t));
    my_heap.free_head = NULL;
    my_heap.free_small_head = NULL;
    my_heap.cache_head = NULL;
    my_heap.cache_pool = NULL;
    my_heap.num_of_data = 0;
    my_heap.average = 0;
    //my_heap.variance = 16;
}



// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().

void* my_malloc_cache()
{
    my_cache_t* metadata = my_heap.cache_head;

    if(!metadata){
        if(my_heap.cache_pool){
            void* ptr = my_heap.cache_pool + 1;
            size_t need_size = my_heap.average + sizeof(my_cache_t) + sizeof(my_is_small_t);
            metadata = (my_cache_t*)((char*)ptr + my_heap.cache_pool->size - need_size);
            if(my_heap.cache_pool->size - need_size >= need_size ){
                my_heap.cache_pool->size -= need_size;
            } else {
                my_heap.cache_pool = NULL;
            }
        } else {
            my_cache_pool_t* pool = (my_cache_pool_t*)mmap_from_system(4096);
            pool->size = 4096 - sizeof(my_cache_pool_t);
            my_heap.cache_pool = pool;
            return my_malloc_cache();
        }
    } else {
        my_heap.cache_head = metadata->next;
    }

    // now, metadata points to the first free slot
    // and prev is the previous entry.


    // |ptr| is the beginning of the allocated object.
    //
    // ... | metadata | object | ...
    //     ^          ^
    //     metadata   ptr
    metadata->next = NULL;
    void* ptr_for_is_small = metadata + 1;
    my_is_small_t* is_small = (my_is_small_t*)((char*)ptr_for_is_small);

    is_small->is_cache = true;

    void* ptr = is_small + 1;

    //printf("%s\n", "give ptr");
    //printf("%lu\n", (unsigned long)metadata);

    return ptr;
}

void* my_malloc_small(size_t size)
{
    my_metadata_small_t* metadata = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    my_metadata_small_t* best_fit = NULL;
    my_metadata_small_t* best_fit_prev = NULL;
    // First-fit: Find the first free slot the object fits.
    // TODO: Update this logic to Best-fit!
    //size_t searching_count = 0;
    while (metadata) {
        if(metadata->size == size){
            best_fit = metadata;
            best_fit_prev = prev;
            break;
        } else if ((!best_fit || metadata->size < best_fit->size) && metadata->size > size) {
            best_fit = metadata;
            best_fit_prev = prev;
        }
        prev = metadata;
        metadata = metadata->next;
    }
    if(size <= my_heap.average && size >= my_heap.average - 15){
        if(!best_fit){
            return my_malloc_cache();
        }
    }
    // now, metadata points to the first free slot
    // and prev is the previous entry.


    if (!best_fit) {
        // There was no free slot available. We need to request a new memory region
        // from the system by calling mmap_from_system().
        //
        //     | metadata | free slot |
        //     ^
        //     metadata
        //     <---------------------->
        //            buffer_size
        size_t buffer_size = 4096;

        my_metadata_small_t* best_fit = (my_metadata_small_t*)mmap_from_system(buffer_size);

        void* ptr_for_is_small = best_fit + 1;
        my_is_small_t* is_small = (my_is_small_t*)((char*)ptr_for_is_small);
        is_small->is_small = true;
        is_small->is_cache = false;

        best_fit->size = buffer_size - sizeof(my_metadata_small_t) - sizeof(my_is_small_t);
        best_fit->next = NULL;
        // Add the memory region to the free list.
        my_add_to_free_list_small(best_fit);
        // Now, try my_malloc() again. This should succeed.
        return my_malloc_small(size);
    }

    // |ptr| is the beginning of the allocated object.
    //
    // ... | metadata | object | ...
    //     ^          ^
    //     metadata   ptr
    void* ptr_for_is_small_of_best_fit = best_fit + 1;
    my_is_small_t* is_small_of_best_fit = (my_is_small_t*)((char*)ptr_for_is_small_of_best_fit);

    void* ptr = is_small_of_best_fit + 1;

    size_t remaining_size = best_fit->size - size;
    // Remove the free slot from the free list.
    my_remove_from_free_list_small(best_fit, best_fit_prev);

    if (remaining_size > sizeof(my_metadata_small_t) + sizeof(my_is_small_t)) {
        //printf("create new metadata for remaining free slot\n");
        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | object | metadata | free slot | ...
        //     ^          ^        ^
        //     metadata   ptr      new_metadata
        //                 <------><---------------------->
        //                   size       remaining size

        best_fit->size = size;
        my_metadata_small_t* new_metadata = (my_metadata_small_t*)((char*)ptr + size);

        my_is_small_t* is_small = (my_is_small_t*)((char*)ptr + size + sizeof(my_metadata_small_t));

        is_small->is_small = true;
        is_small->is_cache = false;

        new_metadata->size = remaining_size - sizeof(my_metadata_small_t) - sizeof(my_is_small_t);
        new_metadata->next = NULL;
        // Add the remaining free slot to the free list.
        my_add_to_free_list_small(new_metadata);
    }
    return ptr;
}


void* my_malloc_large(size_t size)
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
        if(size <= 128){
            if (!best_fit || best_fit->size > 128) {
                return my_malloc_small(size);
            }
        }
        if(size <= my_heap.average && size >= my_heap.average - 15){
            if(!best_fit){
                return my_malloc_cache();
            }
        }
    // now, metadata points to the first free slot
    // and prev is the previous entry.


    if (!best_fit) {
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
        void* ptr_for_is_small = best_fit + 1;
        my_is_small_t* is_small = (my_is_small_t*)((char*)ptr_for_is_small);
        is_small->is_small = false;
        is_small->is_cache = false;

        best_fit->size = buffer_size - sizeof(my_metadata_t) - sizeof(my_is_small_t);

        best_fit->left = NULL;
        best_fit->right = NULL;

        best_fit->before = NULL;
        best_fit->after = NULL;

        best_fit->is_free = true;
        // Add the memory region to the free list.
        my_add_to_free_list(best_fit);
        // Now, try my_malloc() again. This should succeed.
        return my_malloc_large(size);
    }

    // |ptr| is the beginning of the allocated object.
    //
    // ... | metadata | object | ...
    //     ^          ^
    //     metadata   ptr

    void* ptr_for_is_small_of_best_fit = best_fit + 1;
    my_is_small_t* is_small_of_best_fit = (my_is_small_t*)((char*)ptr_for_is_small_of_best_fit);

    void* ptr = is_small_of_best_fit + 1;
    size_t remaining_size = best_fit->size - size;
    best_fit->is_free = false;
    my_remove_from_free_list_with_prev(best_fit, best_fit_prev);
    // Remove the free slot from the free list.
    best_fit->size = size;

    if (remaining_size > sizeof(my_metadata_t) + sizeof(my_is_small_t)) {
        //printf("create new metadata for remaining free slot\n");
        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | is_small | object | metadata | free slot | ...
        //     ^          ^          ^        ^
        //     metadata  is_small   ptr      new_metadata
        //                            <------><---------------------->
        //                              size       remaining size

        my_metadata_t* new_metadata = (my_metadata_t*)((char*)ptr + size);

        my_is_small_t* is_small = (my_is_small_t*)((char*)ptr + size + sizeof(my_metadata_t));
        is_small->is_small = false;
        is_small->is_cache = false;

        new_metadata->size = remaining_size - sizeof(my_metadata_t) - sizeof(my_is_small_t);

        new_metadata->left = NULL;
        new_metadata->right = NULL;
        new_metadata->is_free = true;

        // Add the remaining free slot to the free list.
        my_add_to_free_list(new_metadata);

        assert(best_fit);
        new_metadata->after = best_fit->after;
        new_metadata->before = best_fit;
        if (best_fit->after) {
            best_fit->after->before = new_metadata;
        }
        best_fit->after = new_metadata;
    }
    return ptr;
}

void* my_malloc(size_t size)
{
    if(my_heap.num_of_data < 50){
        my_heap.average = (my_heap.average * my_heap.num_of_data + size) / (++my_heap.num_of_data);        
    } else {
        if(size <= my_heap.average  && size >= my_heap.average - 15){
            if(my_heap.cache_pool || my_heap.cache_head){
                return my_malloc_cache();
            }
        }
    }
    return my_malloc_large(size);
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


    my_is_small_t* is_small = (my_is_small_t*)ptr - 1;
    void* ptr_for_metadata = (void*)(is_small);
    assert(is_small);
    if(is_small->is_cache){
        my_cache_t* metadata = (my_cache_t*) ptr_for_metadata - 1;
        //printf("%s\n", "free");
        //printf("%lu\n", (unsigned long)metadata);
        assert(!metadata->next);
        my_add_to_cache(metadata);
    } else if (is_small->is_small) {
        my_metadata_small_t* metadata = (my_metadata_small_t*)ptr_for_metadata - 1;
        assert(!metadata->next);
        my_merge_left_small(metadata);
        my_merge_right_or_add_small(metadata);
    } else {
        my_metadata_t* metadata = (my_metadata_t*)ptr_for_metadata - 1;
        assert(metadata);

        metadata = my_fill_gap(metadata);
        metadata->right = NULL;
        metadata->left = NULL;
        metadata->is_free = true;
        // Add the free slot to the free list.
        my_merge_left(metadata);
        my_merge_right(metadata);
    }
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
