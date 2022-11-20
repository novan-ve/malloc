#ifndef MALLOC_H
# define MALLOC_H

# include <unistd.h>
# include <pthread.h>

# define ALIGNMENT 16
# define CHUNKS_PER_ZONE 100
# define MAX_TINY_CHUNK 256
# define MAX_SMALL_CHUNK 4096

extern void *zones[3];
extern pthread_mutex_t mutex;

typedef struct  s_heap_header {
    void        *next_heap;
    void        *prev_heap;
}               heap_header;

typedef struct  s_chunk_header {
    size_t      size;
    char        allocated;
}               chunk_header;

typedef struct  s_chunk_footer {
    size_t      size;
    int         filler;
}               chunk_footer;

void            free(void *ptr);
void            *malloc(size_t size);
void            *realloc(void *ptr, size_t size);
void            show_alloc_mem();
void            show_alloc_mem_ex();

void            *calloc(size_t count, size_t size);

size_t          align(size_t size, int alignment);
unsigned long   chunk_overhead(void);
unsigned long   heap_overhead(void);
size_t          get_size(void *ptr);
char            get_alloc(void *header_ptr);
void            set_size(void *ptr, size_t size);
void            set_alloc(void *header_ptr, char alloc);
void            *header_ptr(void *chunk_ptr);
void            *footer_ptr(void *chunk_ptr);
void            *next_chunk_ptr(void *chunk_ptr);
void            *prev_chunk_ptr(void *chunk_ptr);
void            *first_chunk_ptr(void *heap_ptr);
void            *get_prev_heap(void *heap_header_ptr);
void            *get_next_heap(void *heap_header_ptr);
void            set_prev_heap(void *heap_header_ptr, void *heap_ptr);
void            set_next_heap(void *heap_header_ptr, void *heap_ptr);
size_t          get_zone_size(size_t chunk_size);
int             get_zone_from_chunk(void *chunk_ptr);
int             get_zone_from_size(size_t chunk_size);
unsigned long   allocated_bytes();

void            *ft_memcpy(void *dst, const void *src, size_t n);
int             ft_memcmp(const void *s1, const void *s2, size_t n);

#endif