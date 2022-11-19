# Malloc
C implementation of the stdlib malloc functions. It creates the `libft_malloc.so` library, which can be used to replace malloc in all system binaries.
The following functions are implemented:

```c
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t count, size_t size);
void *realloc(void *ptr, size_t size);
```
