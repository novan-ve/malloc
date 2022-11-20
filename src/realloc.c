#include "malloc.h"

void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        void *new_ptr = malloc(size);
        if (new_ptr) {
            return (new_ptr);
        }
        return (ptr);
    }
    pthread_mutex_lock(&mutex);
    if (get_zone_from_chunk(ptr) == -1) {
        pthread_mutex_unlock(&mutex);
        return (NULL);
    }
    if (!size) {
        pthread_mutex_unlock(&mutex);
        free(ptr);
        return (NULL);
    }

    size_t chunk_size = get_size(header_ptr(ptr));
    size_t new_size = align(size + chunk_overhead(), ALIGNMENT);
    size_t next_size = get_size(header_ptr(next_chunk_ptr(ptr)));
    size_t next_alloc = get_alloc(header_ptr(next_chunk_ptr(ptr)));
    size_t available = chunk_size + (!next_alloc * next_size);
    int zone = get_zone_from_size(chunk_size - chunk_overhead());

    if (new_size == chunk_size) {
        pthread_mutex_unlock(&mutex);
        return (ptr);
    }
    if (new_size > available || zone == 2 || zone != get_zone_from_size(size)) {
        pthread_mutex_unlock(&mutex);
        void *new_ptr = malloc(size);
        if (new_ptr) {
            if (new_size >= available) {
                ft_memcpy(new_ptr, ptr, chunk_size - chunk_overhead());
            }
            else {
                ft_memcpy(new_ptr, ptr, size);
            }
            free(ptr);
        }
        return (new_ptr);
    }
    size_t extra_size = available - new_size;
    if (extra_size && extra_size < align(1 + chunk_overhead(), ALIGNMENT)) {
        set_size(header_ptr(ptr), available);
        set_size(footer_ptr(ptr), available);
    }
    else {
        set_size(header_ptr(ptr), new_size);
        set_size(footer_ptr(ptr), new_size);
        if (extra_size) {
            set_size(header_ptr(next_chunk_ptr(ptr)), extra_size);
            set_size(footer_ptr(next_chunk_ptr(ptr)), extra_size);
            set_alloc(header_ptr(next_chunk_ptr(ptr)), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
    return (ptr);
}
