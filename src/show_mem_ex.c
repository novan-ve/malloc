#include "malloc.h"
#include <stdio.h>

int check_duplicates(unsigned char *addr, char *buf, size_t i, size_t len) {
    static int duplicate = 0;

    if (i < 16) {
        return (0);
    }
    if (!duplicate) {
        printf("  |%s|\n", buf);
    }
    if (i + 16 < len) {
        if (!ft_memcmp(addr + i, addr + i - 16, 16)) {
            if (!duplicate) {
                printf("*\n");
            }
            duplicate = 1;
        }
        else {
            duplicate = 0;
        }
    }
    return (duplicate);
}

void hexdump(void *b, size_t len) {
    unsigned char *addr = (unsigned char *)b;
    char buf[17];
    size_t i = 0;

    buf[0] = '\0';
    while (i < len) {
        if (i % 16 == 0) {
            if (check_duplicates(addr, buf, i, len)) {
                i += 16;
                continue;
            }
            printf("%08zx ", i);
        }
        else if (i % 8 == 0) {
            printf(" ");
        }
        printf(" %02x", addr[i]);
        if (addr[i] < 32 || addr[i] > 126) {
            buf[i % 16] = '.';
        }
        else {
            buf[i % 16] = addr[i];
        }
        buf[i % 16 + 1] = '\0';
        i++;
    }
    while (i % 16) {
        printf("   ");
        i++;
    }
    printf("  |%s|\n", buf);
}

void show_alloc_mem_ex() {
    void *heap;
    void *chunk;
    size_t size;

    printf("TINY :\n");
    heap = zones[0];
    while (heap) {
        hexdump(header_ptr(heap), get_zone_size(MAX_TINY_CHUNK));
        heap = get_next_heap(header_ptr(heap));
    }
    if (zones[0]) {
        printf("\n");
    }

    printf("SMALL :\n");
    heap = zones[1];
    while (heap) {
        hexdump(header_ptr(heap), get_zone_size(MAX_SMALL_CHUNK));
        heap = get_next_heap(header_ptr(heap));
    }
    if (zones[1]) {
        printf("\n");
    }

    printf("LARGE :\n");
    heap = zones[2];
    while (heap) {
        chunk = next_chunk_ptr(first_chunk_ptr(heap));
        size = get_size(header_ptr(chunk)) - chunk_overhead();
        hexdump(header_ptr(heap), get_zone_size(size));
        heap = get_next_heap(header_ptr(heap));
    }
}
