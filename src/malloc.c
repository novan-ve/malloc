#include "malloc.h"
#include <sys/mman.h>
#include <sys/resource.h>

void *zones[3] = {NULL, NULL, NULL};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void allocate_chunk(void *chunk_ptr, size_t size) {
    size_t extra_size = get_size(header_ptr(chunk_ptr)) - size;

    if (extra_size >= align(1 + chunk_overhead(), ALIGNMENT)) {
        set_size(header_ptr(chunk_ptr), size);
        set_size(footer_ptr(chunk_ptr), size);
        set_size(header_ptr(next_chunk_ptr(chunk_ptr)), extra_size);
        set_size(footer_ptr(next_chunk_ptr(chunk_ptr)), extra_size);
        set_alloc(header_ptr(next_chunk_ptr(chunk_ptr)), 0);
    }
    set_alloc(header_ptr(chunk_ptr), 1);
}

void *extend(size_t new_size, int zone_i) {
    void *prev_heap_ptr = zones[zone_i];
    void *new_heap_ptr;
    size_t heap_size = get_zone_size(new_size - chunk_overhead());
    struct rlimit rl;

    getrlimit(RLIMIT_AS, &rl);
    if (heap_size + allocated_bytes() > rl.rlim_cur) {
        return (NULL);
    }

    new_heap_ptr = mmap(NULL, heap_size, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE | MAP_ANON, -1, 0);

    if (new_heap_ptr == MAP_FAILED) {
        return (NULL);
    }

    new_heap_ptr += sizeof(heap_header);

    if (zones[zone_i]) {
        while (get_next_heap(header_ptr(prev_heap_ptr))) {
            prev_heap_ptr = get_next_heap(header_ptr(prev_heap_ptr));
        }
        set_next_heap(header_ptr(prev_heap_ptr), new_heap_ptr);
    }
    else {
        zones[zone_i] = new_heap_ptr;
    }

    set_prev_heap(header_ptr(new_heap_ptr), prev_heap_ptr);
    set_next_heap(header_ptr(new_heap_ptr), NULL);

    void *first_chunk = first_chunk_ptr(new_heap_ptr);

    set_alloc(header_ptr(first_chunk), 0);
    set_size(header_ptr(first_chunk), heap_size - heap_overhead() + chunk_overhead());
    set_size(footer_ptr(first_chunk), heap_size - heap_overhead() + chunk_overhead());

    set_alloc(header_ptr(next_chunk_ptr(first_chunk)), 1);
    set_size(header_ptr(next_chunk_ptr(first_chunk)), 0);

    allocate_chunk(first_chunk, align(chunk_overhead(), ALIGNMENT));

    return (next_chunk_ptr(first_chunk));
}

void *malloc(size_t size) {
    size_t new_size = align(size + chunk_overhead(), ALIGNMENT);
    void *chunk_ptr;
    int zone_i = get_zone_from_size(size);
    void *heap = zones[zone_i];

    if (size > SIZE_MAX - getpagesize() * 2) {
        return (NULL);
    }
    pthread_mutex_lock(&mutex);
    if (zone_i != 2) {
        while (heap) {
            chunk_ptr = first_chunk_ptr(heap);

            while (get_size(header_ptr(chunk_ptr)) != 0) {
                if (!get_alloc(header_ptr(chunk_ptr)) &&
                    get_size(header_ptr(chunk_ptr)) >= new_size)
                {
                    allocate_chunk(chunk_ptr, new_size);
                    pthread_mutex_unlock(&mutex);
                    return (chunk_ptr);
                }
                chunk_ptr = next_chunk_ptr(chunk_ptr);
            }
            heap = get_next_heap(header_ptr(heap));
        }
    }

    chunk_ptr = extend(new_size, zone_i);
    if (chunk_ptr) {
        allocate_chunk(chunk_ptr, new_size);
    }
    pthread_mutex_unlock(&mutex);
    return (chunk_ptr);
}

void *calloc(size_t count, size_t size) {
    void *ptr = malloc(count * size);

    if (ptr) {
        void *s = ptr;
        size_t n = count * size;

        while (n) {
            *(unsigned char*)s = 0;
            s++;
            n--;
        }
    }

    return (ptr);
}
