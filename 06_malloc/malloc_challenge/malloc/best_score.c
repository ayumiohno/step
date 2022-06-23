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

typedef struct my_type_of_metadata_t {
    bool is_small;
    bool is_cache;
} my_type_of_metadata_t;

typedef struct my_metadata_t {  //binary tree
    size_t size;
    struct my_metadata_t* left;    //metadata* of smaller size
    struct my_metadata_t* right;   //metadata* of larger size
    struct my_metadata_t* before;  //metadata* of previous pointer
    struct my_metadata_t* after;   //metadata* of next pointer
    bool is_free;
} my_metadata_t;

typedef struct my_metadata_small_t {  //linked list
    size_t size;
    struct my_metadata_small_t* next;
} my_metadata_small_t;

typedef struct my_cache_t {  //cache; size : my_heap_t.average
    struct my_cache_t* next;
} my_cache_t;

typedef struct my_cache_pool_t {  //free region saved for cache
    size_t size;
} my_cache_pool_t;

typedef struct my_heap_t {
    my_metadata_t* free_head;
    my_metadata_small_t* free_small_head;
    my_cache_t* cache_head;
    my_cache_pool_t* cache_pool;
    size_t num_of_data;  //50になるまで更新
    size_t average;      //num_of_dateが50になるまで更新
} my_heap_t;


//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap;

//
// Helper functions (feel free to add/remove/edit!)
//

//
// Helper functions for binary tree (my_metadata_t)
//

//targetをmetadataの部分木にleafとして挿入
void my_insert_node(my_metadata_t* metadata, my_metadata_t* target)
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

//targetをbinary_treeに挿入
void my_add_to_binary_tree(my_metadata_t* metadata)
{
    assert(metadata);
    if (!my_heap.free_head) {
        my_heap.free_head = metadata;
    } else {
        my_insert_node(my_heap.free_head, metadata);
    }
}

//targetをbinary_treeから除去. prevはtargetのparent
void my_remove_from_binary_tree_with_prev(my_metadata_t* metadata, my_metadata_t* prev)
{
    assert(metadata);
    if (prev) {
        if (metadata->right) {
            if (metadata->size < prev->size) {
                prev->left = metadata->right;
            } else {
                prev->right = metadata->right;
            }
            my_insert_node(metadata->right, metadata->left);
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
            my_insert_node(metadata->right, metadata->left);
        } else {
            my_heap.free_head = metadata->left;
        }
    }
}

//targetをbinary_treeから除去. prevは二部探索で求める.
void my_remove_from_binary_tree(my_metadata_t* metadata)
{
    my_metadata_t* node = my_heap.free_head;
    my_metadata_t* prev = NULL;
    assert(metadata);
    while (node) {
        if (metadata == node) {
            my_remove_from_binary_tree_with_prev(metadata, prev);
            return;
        } else if (metadata->size < node->size) {
            prev = node;
            node = node->left;
        } else {
            prev = node;
            node = node->right;
        }
    }
    assert(false);
}

//右側とmerge
void my_merge_right(my_metadata_t* metadata)
{
    //
    // ... | metadata | type | free slot | after |  type | free slot | ...
    //     ^                 ^         ^
    //     metadata          <----metadata->size(after merge)-------->
    //                       <-----------><------><------><---------->
    //                  metadata->size(before merge)
    //
    if (!metadata->after || !metadata->after->is_free) {
        return;
    }

    my_remove_from_binary_tree(metadata->after);

    metadata->size += (sizeof(my_metadata_t) + sizeof(my_type_of_metadata_t) + metadata->after->size);

    my_metadata_t* after = metadata->after->after;

    //吸収されるmetadataを空に
    metadata->after->after = NULL;
    metadata->after->before = NULL;
    metadata->after->right = NULL;
    metadata->after->left = NULL;
    metadata->after->size = 0;
    metadata->after->is_free = 0;

    metadata->after = after;

    if (metadata->after) {
        metadata->after->before = metadata;
    }
}

//左側とmerge
//mergeできたら左側と一緒にtreeへ挿入
//mergeできなかったらそのままtreeへ挿入
void my_merge_left_or_add(my_metadata_t* metadata)
{
    //
    // ... | before | type | free slot | metadata |  type | free slot | ...
    //                     <-----before->size(after merge)------------>
    //                     <-----------><--------><-------><---------->
    //                  before->size(before merge)
    //

    if (metadata->before && metadata->before->is_free) {

        my_remove_from_binary_tree(metadata->before);

        metadata = metadata->before;

        metadata->left = NULL;
        metadata->right = NULL;

        metadata->size += (sizeof(my_metadata_t) + sizeof(my_type_of_metadata_t) + metadata->after->size);

        my_metadata_t* after = metadata->after->after;

        //吸収されるmetadataを空に
        metadata->after->after = NULL;
        metadata->after->before = NULL;
        metadata->after->right = NULL;
        metadata->after->left = NULL;
        metadata->after->size = 0;
        metadata->after->is_free = 0;

        metadata->after = after;
        if (metadata->after) {
            metadata->after->before = metadata;
        }
    }

    my_add_to_binary_tree(metadata);
}

//
// Helper functions for free list (my_metadata_small_t)
//

//linked listの冒頭に挿入
void my_add_to_free_list(my_metadata_small_t* metadata)
{
    assert(!metadata->next);
    metadata->next = my_heap.free_small_head;
    my_heap.free_small_head = metadata;
}

//linked listから削除
void my_remove_from_free_list(my_metadata_small_t* metadata, my_metadata_small_t* prev)
{
    if (prev) {
        prev->next = metadata->next;
    } else {
        my_heap.free_small_head = metadata->next;
    }
    metadata->next = NULL;
}

//右とmerge
void my_merge_right_small(my_metadata_small_t* metadata)
{
    //
    // ... | metadata | type | free slot | next_metadata |  type | free slot | ...
    //                ^
    //           ptr_for_metadata
    //
    void* ptr_after_metadata = metadata + 1;
    my_metadata_small_t* next_metadata = (my_metadata_small_t*)((char*)ptr_after_metadata + sizeof(my_type_of_metadata_t) + metadata->size);

    my_metadata_small_t* node = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    while (node) {
        if (node == next_metadata) {
            my_remove_from_free_list(next_metadata, prev);
            metadata->size += next_metadata->size + sizeof(my_metadata_small_t) + sizeof(my_type_of_metadata_t);
            //吸収されるmetadataを空に
            next_metadata->next = NULL;
            next_metadata->size = 0;
            return;
        } else {
            prev = node;
            node = node->next;
        }
    }
}

//左側とmerge
//mergeできたら左側と一緒にtreeへ挿入
//mergeできなかったらそのままtreeへ挿入
void my_merge_left_or_add_small(my_metadata_small_t* metadata)
{
    //
    // ... | node | type | free slot | ... | metadata |  type | free slot | ...
    //            ^                        ^
    //       ptr_after_node          ptr_of_metadata
    //
    // ..   |    before | type | free slot | metadata |  type | free slot | ...
    //                  ^                  ^
    //            ptr_after_node      ptr_of_metadata
    //                          <-size----><------+=size------------------>
    //
    void* ptr_of_metadata = metadata;
    my_metadata_small_t* node = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    while (node) {
        void* ptr_after_node = node + 1;
        if ((char*)ptr_of_metadata == (char*)ptr_after_node + sizeof(my_type_of_metadata_t) + node->size) {
            my_remove_from_free_list(node, prev);
            node->size += metadata->size + sizeof(my_metadata_small_t) + sizeof(my_type_of_metadata_t);
            my_add_to_free_list(node);
            //吸収されるmetadataを空に
            metadata->next = NULL;
            metadata->size = 0;
            return;
        } else {
            prev = node;
            node = node->next;
        }
    }
    my_add_to_free_list(metadata);
}

//
// Helper functions for free list (my_metadata_small_t)
//

//cacheの冒頭に挿入
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
    my_heap.free_head = NULL;
    my_heap.free_small_head = NULL;
    my_heap.cache_head = NULL;
    my_heap.cache_pool = NULL;
    my_heap.num_of_data = 0;
    my_heap.average = 0;
}


// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().

void* my_malloc_cache()
{
    my_cache_t* metadata = my_heap.cache_head;

    if (!metadata) {
        if (my_heap.cache_pool) {  //poolの末尾からmy_cache_tをとってくる
            void* ptr = my_heap.cache_pool + 1;
            size_t need_size = my_heap.average + sizeof(my_cache_t) + sizeof(my_type_of_metadata_t);
            metadata = (my_cache_t*)((char*)ptr + my_heap.cache_pool->size - need_size);
            //
            //               <------------cache_pool->size-------------->
            //  | cachepool |            | metadata |  type |   object   |
            //                            <---------need_size------------>
            //
            if (my_heap.cache_pool->size - need_size >= need_size) {
                my_heap.cache_pool->size -= need_size;
            } else {
                my_heap.cache_pool = NULL;
            }
        } else {  //poolがNULLの時は新たに4096bit取ってくる
            my_cache_pool_t* pool = (my_cache_pool_t*)mmap_from_system(4096);
            pool->size = 4096 - sizeof(my_cache_pool_t);
            my_heap.cache_pool = pool;
            return my_malloc_cache();
        }
    } else {  //freeなcacheが残っている場合
        my_heap.cache_head = metadata->next;
    }


    //
    // ... | metadata | type        | object | ...
    //     ^          ^             ^
    //     metadata  ptr_for_type   ptr
    //

    metadata->next = NULL;
    void* ptr_for_type = metadata + 1;
    my_type_of_metadata_t* type = (my_type_of_metadata_t*)((char*)ptr_for_type);

    type->is_cache = true;

    void* ptr = type + 1;

    return ptr;
}

void* my_malloc_small(size_t size)
{
    my_metadata_small_t* metadata = my_heap.free_small_head;
    my_metadata_small_t* prev = NULL;
    my_metadata_small_t* best_fit = NULL;
    my_metadata_small_t* best_fit_prev = NULL;

    while (metadata) {
        if (metadata->size == size) {
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
    if (my_heap.num_of_data >= 50 && size <= my_heap.average && size >= my_heap.average - 15) {
        if (!best_fit) {
            return my_malloc_cache();
        }
    }

    if (!best_fit) {
        // There was no free slot available. We need to request a new memory region
        // from the system by calling mmap_from_system().
        //
        //     | metadata | type | free slot |
        //     ^
        //     metadata
        //     <----------------------------->
        //              buffer_size
        size_t buffer_size = 4096;

        my_metadata_small_t* best_fit = (my_metadata_small_t*)mmap_from_system(buffer_size);

        void* ptr_for_is_small = best_fit + 1;
        my_type_of_metadata_t* is_small = (my_type_of_metadata_t*)((char*)ptr_for_is_small);
        is_small->is_small = true;
        is_small->is_cache = false;

        best_fit->size = buffer_size - sizeof(my_metadata_small_t) - sizeof(my_type_of_metadata_t);
        best_fit->next = NULL;
        // Add the memory region to the free list.
        my_add_to_free_list(best_fit);
        // Now, try my_malloc() again. This should succeed.
        return my_malloc_small(size);
    }

    //
    // ... | metadata | type        | object | ...
    //     ^          ^             ^
    //     metadata  ptr_for_type   ptr
    //

    void* ptr_for_type_of_best_fit = best_fit + 1;
    my_type_of_metadata_t* type_of_best_fit = (my_type_of_metadata_t*)((char*)ptr_for_type_of_best_fit);

    void* ptr = type_of_best_fit + 1;

    size_t remaining_size = best_fit->size - size;
    // Remove the free slot from the free list.
    my_remove_from_free_list(best_fit, best_fit_prev);

    if (remaining_size > sizeof(my_metadata_small_t) + sizeof(my_type_of_metadata_t)) {

        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | type |  object | metadata |  type | free slot | ...
        //     ^                 ^         ^
        //     metadata         ptr    new_metadata
        //                       <--------><------------------------------>
        //                           size       remaining size

        best_fit->size = size;
        my_metadata_small_t* new_metadata = (my_metadata_small_t*)((char*)ptr + size);

        my_type_of_metadata_t* type = (my_type_of_metadata_t*)((char*)ptr + size + sizeof(my_metadata_small_t));

        type->is_small = true;
        type->is_cache = false;

        new_metadata->size = remaining_size - sizeof(my_metadata_small_t) - sizeof(my_type_of_metadata_t);
        new_metadata->next = NULL;
        // Add the remaining free slot to the free list.
        my_add_to_free_list(new_metadata);
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
    if (size <= 128) {
        if (!best_fit || best_fit->size > 128) {
            return my_malloc_small(size);
        }
    }
    if (my_heap.num_of_data >= 50 && size <= my_heap.average && size >= my_heap.average - 15) {
        if (!best_fit) {
            return my_malloc_cache();
        }
    }
    // now, metadata points to the first free slot
    // and prev is the previous entry.


    if (!best_fit) {
        size_t buffer_size = 4096;
        my_metadata_t* best_fit = (my_metadata_t*)mmap_from_system(buffer_size);
        void* ptr_for_is_small = best_fit + 1;
        my_type_of_metadata_t* is_small = (my_type_of_metadata_t*)((char*)ptr_for_is_small);
        is_small->is_small = false;
        is_small->is_cache = false;

        best_fit->size = buffer_size - sizeof(my_metadata_t) - sizeof(my_type_of_metadata_t);

        best_fit->left = NULL;
        best_fit->right = NULL;

        best_fit->before = NULL;
        best_fit->after = NULL;
        best_fit->is_free = true;

        // Add the memory region to the free list.
        my_add_to_binary_tree(best_fit);
        // Now, try my_malloc() again. This should succeed.
        return my_malloc_large(size);
    }

    //
    // ... | metadata | type        | object | ...
    //     ^          ^             ^
    //     metadata  ptr_for_type   ptr
    //

    void* ptr_for_type_of_best_fit = best_fit + 1;
    my_type_of_metadata_t* type_of_best_fit = (my_type_of_metadata_t*)((char*)ptr_for_type_of_best_fit);

    void* ptr = type_of_best_fit + 1;
    size_t remaining_size = best_fit->size - size;
    best_fit->is_free = false;
    my_remove_from_binary_tree_with_prev(best_fit, best_fit_prev);

    if (remaining_size > sizeof(my_metadata_t) + sizeof(my_type_of_metadata_t)) {
        // Create a new metadata for the remaining free slot.
        //
        // ... | metadata | type |  object | metadata |  type | free slot | ...
        //     ^                 ^         ^
        //     metadata         ptr    new_metadata
        //                       <--------><------------------------------>
        //                           size       remaining size
        best_fit->size = size;

        my_metadata_t* new_metadata = (my_metadata_t*)((char*)ptr + size);

        my_type_of_metadata_t* type = (my_type_of_metadata_t*)((char*)ptr + size + sizeof(my_metadata_t));
        type->is_small = false;
        type->is_cache = false;

        new_metadata->size = remaining_size - sizeof(my_metadata_t) - sizeof(my_type_of_metadata_t);

        new_metadata->left = NULL;
        new_metadata->right = NULL;
        new_metadata->is_free = true;

        // Add the remaining free slot to the free list.
        my_add_to_binary_tree(new_metadata);

        //insert new_metadata between metadata and metadata->after
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
    if (my_heap.num_of_data < 50) {  //averageを更新
        my_heap.average = (my_heap.average * my_heap.num_of_data + size) / (++my_heap.num_of_data);
    } else {  //cacheの利用開始
        if (size <= my_heap.average && size >= my_heap.average - 15) {
            if (my_heap.cache_pool || my_heap.cache_head) {
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
    // ... | metadata | type | object | ...
    //     ^                 ^
    //  ptr for metadata    ptr


    my_type_of_metadata_t* type = (my_type_of_metadata_t*)ptr - 1;
    void* ptr_for_metadata = (void*)(type);
    assert(type);
    if (type->is_cache) {
        my_cache_t* metadata = (my_cache_t*)ptr_for_metadata - 1;
        assert(!metadata->next);
        my_add_to_cache(metadata);
    } else if (type->is_small) {
        my_metadata_small_t* metadata = (my_metadata_small_t*)ptr_for_metadata - 1;
        assert(!metadata->next);
        my_merge_right_small(metadata);
        my_merge_left_or_add_small(metadata);
    } else {
        my_metadata_t* metadata = (my_metadata_t*)ptr_for_metadata - 1;
        assert(metadata);

        metadata->right = NULL;
        metadata->left = NULL;
        metadata->is_free = true;

        my_merge_right(metadata);
        my_merge_left_or_add(metadata);
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
