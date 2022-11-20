#include "malloc.h"
#include <sys/mman.h>

void *defragment(void *ptr) {
    size_t prev_alloc = get_alloc(header_ptr(prev_chunk_ptr(ptr)));
    size_t next_alloc = get_alloc(header_ptr(next_chunk_ptr(ptr)));
    size_t size = get_size(header_ptr(ptr));

    if (prev_alloc && !next_alloc) {
        size += get_size(header_ptr(next_chunk_ptr(ptr)));
        set_size(header_ptr(ptr), size);
        set_size(footer_ptr(ptr), size);
    }
    else if (!prev_alloc && next_alloc) {
        size += get_size(header_ptr(prev_chunk_ptr(ptr)));
        set_size(footer_ptr(ptr), size);
        set_size(header_ptr(prev_chunk_ptr(ptr)), size);
        ptr = prev_chunk_ptr(ptr);
    }
    else if (!prev_alloc && !next_alloc) {
        size += get_size(header_ptr(prev_chunk_ptr(ptr))) +
                get_size(header_ptr(next_chunk_ptr(ptr)));
        set_size(header_ptr(prev_chunk_ptr(ptr)), size);
        set_size(footer_ptr(next_chunk_ptr(ptr)), size);
        ptr = prev_chunk_ptr(ptr);
    }

    return (ptr);
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    pthread_mutex_lock(&mutex);
    int zone_i = get_zone_from_chunk(ptr);
    if (zone_i != -1 && get_alloc(header_ptr(ptr))) {
        set_alloc(header_ptr(ptr), 0);
        ptr = defragment(ptr);

        size_t size = get_size(header_ptr(ptr));
        size_t next_size = get_size(header_ptr(next_chunk_ptr(ptr)));

        if (!next_size && prev_chunk_ptr(ptr) == first_chunk_ptr(zones[zone_i])) {
            void *heap = header_ptr(prev_chunk_ptr(ptr));
            void *prev_heap = get_prev_heap(header_ptr(heap));
            void *next_heap = get_next_heap(header_ptr(heap));

            if (prev_heap || next_heap) {
                if (next_heap) {
                    set_prev_heap(header_ptr(next_heap), prev_heap);
                }
                if (prev_heap) {
                    set_next_heap(header_ptr(prev_heap), next_heap);
                }
                if (heap == zones[zone_i]) {
                    zones[zone_i] = next_heap;
                }

                if (munmap(header_ptr(heap), align(size + heap_overhead(), getpagesize()))) {
                    write(2, "free: munmap failed\n", 20);
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}