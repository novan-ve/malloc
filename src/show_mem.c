#include "malloc.h"

void ft_putstr(char *s) {
    if (s) {
        while (*s) {
            write(1, s++, 1);
        }
    }
}

void ft_putnbr(unsigned long n) {
    if (n > 9) {
        ft_putnbr(n / 10);
        ft_putnbr(n % 10);
    }
    else {
        char ch = n + '0';
        write(1, &ch, 1);
    }
}

void ft_putaddr(void *b) {
    unsigned long addr = (unsigned long)b;
    char str[64];
    int inc;
    int i = 0;

    write(1, "0x", 2);
    if (!b) {
        write(1, "0", 1);
    }
    else {
        while (addr != 0 && i < 64) {
            inc = (addr % 16 < 10) ? 48 : 55;
            str[i] = addr % 16 + inc;
            addr /= 16;
            i++;
        }
        for (i = i - 1; i >= 0; i--) {
            write(1, &str[i], 1);
        }
    }
}

void show_alloc_mem() {
    unsigned long   total = 0;
    unsigned long   size = 0;
    char            *zone_names[3] = {"TINY", "SMALL", "LARGE"};
    void            *heap;
    void            *chunk;

    for (int i = 0; i < 3; i++) {
        ft_putstr(zone_names[i]);
        ft_putstr(" : ");
        if (!zones[i]) {
            ft_putstr("0x0\n");
            continue;
        }
        ft_putaddr(header_ptr(zones[i]));
        ft_putstr("\n");

        heap = zones[i];
        while (heap) {
            chunk = next_chunk_ptr(first_chunk_ptr(heap));
            while (get_size(header_ptr(chunk))) {
                if (get_alloc(header_ptr(chunk))) {
                    size = get_size(header_ptr(chunk)) - chunk_overhead();
                    total += size;

                    ft_putaddr(chunk);
                    ft_putstr(" - ");
                    ft_putaddr(footer_ptr(chunk));
                    ft_putstr(" : ");
                    ft_putnbr(size);
                    ft_putstr(" bytes\n");
                }
                chunk = next_chunk_ptr(chunk);
            }
            heap = get_next_heap(header_ptr(heap));
        }
    }
    ft_putstr("Total : ");
    ft_putnbr(total);
    ft_putstr(" bytes\n");
}
