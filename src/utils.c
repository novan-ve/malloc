#include "malloc.h"

size_t align(size_t size, int alignment) {
    return ((size + (alignment - 1)) & ~(alignment - 1));
}

unsigned long chunk_overhead(void) {
    return (sizeof(chunk_header) + sizeof(chunk_footer));
}

unsigned long heap_overhead(void) {
    return (sizeof(heap_header) + sizeof(chunk_header) + chunk_overhead());
}

size_t get_size(void *ptr) {
    return (((chunk_header*)ptr)->size);
}

char get_alloc(void *header_ptr) {
    return (((chunk_header*)header_ptr)->allocated);
}

void set_size(void *ptr, size_t size) {
    ((chunk_header*)ptr)->size = size;
}

void set_alloc(void *header_ptr, char alloc) {
    ((chunk_header*)header_ptr)->allocated = alloc;
}

void *header_ptr(void *chunk_ptr) {
    return ((char*)chunk_ptr - sizeof(chunk_header));
}

void *footer_ptr(void *chunk_ptr) {
    return ((char*)chunk_ptr + (get_size(header_ptr(chunk_ptr)) - chunk_overhead()));
}

void *next_chunk_ptr(void *chunk_ptr) {
    return ((char*)chunk_ptr + get_size(header_ptr(chunk_ptr)));
}

void *prev_chunk_ptr(void *chunk_ptr) {
    return ((char*)chunk_ptr - get_size((char*)chunk_ptr - chunk_overhead()));
}

void *first_chunk_ptr(void *heap_ptr) {
    return ((char*)heap_ptr + sizeof(chunk_header));
}

void *get_prev_heap(void *heap_header_ptr) {
    return (((heap_header*)heap_header_ptr)->prev_heap);
}

void *get_next_heap(void *heap_header_ptr) {
    return (((heap_header*)heap_header_ptr)->next_heap);
}

size_t get_zone_size(size_t chunk_size) {
    int zone_index = get_zone_from_size(chunk_size);
    size_t total_size;

    if (zone_index == 0) {
        total_size = (MAX_TINY_CHUNK + chunk_overhead()) * CHUNKS_PER_ZONE + heap_overhead();
        return (align(total_size, getpagesize()));
    }
    if (zone_index == 1) {
        total_size = (MAX_SMALL_CHUNK + chunk_overhead()) * CHUNKS_PER_ZONE + heap_overhead();
        return (align(total_size, getpagesize()));
    }
    return (align(chunk_size + heap_overhead() + chunk_overhead(), getpagesize()));
}

int get_zone_from_size(size_t chunk_size) {
    if (chunk_size <= MAX_TINY_CHUNK) {
        return (0);
    }
    else if (chunk_size <= MAX_SMALL_CHUNK) {
        return (1);
    }
    return (2);
}

int get_zone_from_chunk(void *chunk_ptr) {
    void *heap;
    void *chunk;

    for (int i = 0; i < 3; i++) {
        heap = zones[i];
        while (heap) {
            chunk = first_chunk_ptr(heap);
            while (get_size(header_ptr(chunk)) != 0) {
                if (chunk == chunk_ptr) {
                    return (i);
                }
                chunk = next_chunk_ptr(chunk);
            }
            heap = get_next_heap(header_ptr(heap));
        }
    }
    return (-1);
}

void set_prev_heap(void *heap_header_ptr, void *heap_ptr) {
    ((heap_header*)heap_header_ptr)->prev_heap = heap_ptr;
}

void set_next_heap(void *heap_header_ptr, void *heap_ptr) {
    ((heap_header*)heap_header_ptr)->next_heap = heap_ptr;
}

unsigned long allocated_bytes() {
    unsigned long total = 0;
    void *heap;
    void *chunk;

    for (int i = 0; i < 3; i++) {
        heap = zones[i];
        while (heap) {
            if (get_size(first_chunk_ptr(heap)) != 0) {
                chunk = next_chunk_ptr(first_chunk_ptr(heap));
                total += get_zone_size(get_size(header_ptr(chunk)));
            }
            heap = get_next_heap(header_ptr(heap));
        }
    }
    return (total);
}

void *ft_memcpy(void *dst, const void *src, size_t n) {
    char        *d = (char*)dst;
    const char  *s = (char*)src;

    if (d && s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    }

    return (dst);
}

int ft_memcmp(const void *s1, const void *s2, size_t n) {
    unsigned const char *str1 = s1;
    unsigned const char *str2 = s2;

    while (n != 0) {
        if (*str1 != *str2)
            return (*str1 - *str2);
        str1++;
        str2++;
        n--;
    }
    return (0);
}

