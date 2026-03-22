#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define ALIGN(size) (((size) + 7) & ~7)
#define BLOCK_SIZE sizeof(struct s_block)

typedef struct s_block {
    size_t size;
    int free;
    int debug;
    struct s_block *next;
} t_block;

void *base = NULL;