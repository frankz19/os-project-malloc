#include <unistd.h> // Para sbrk
#include "mm_malloc.h"
#include <sys/types.h>
#include <stddef.h>

// Inicio de la lista enlazada del heap
void *base = NULL;

#define ALIGN(size) (((size) + 7) & ~7)


//extra functions

 /* 
 * request_space: extend heap request memory to kernel
 * last: pointer to last actually element.
 * size: align size request by user.
 * return: pointer to new element or null.
 * 
 * 
 */

static block_meta *request_space(block_meta *last, size_t size) {
    block_meta *block;

    block = sbrk(0);

    void *request = sbrk(size + META_SIZE); //use void to any pointer var type

    if (request == (void*) -1) {
        return NULL;
    }

    if (last) {
        last->next = block;
    }

    block->size = size;
    block->next = NULL;
    block->free = 0;          
    block->magic = 0x4652414E; //security sign

    return block;
}
//extra functions





void *my_malloc(size_t size) {
   block_meta *block;
    size_t s;

    if (size <= 0) {
        return NULL;
    }

    s = ALIGN(size);
    
    if (base == NULL) {

        block = request_space(NULL, s);
        
        if (!block) {
            return NULL; // kernel say no more ram
        }

        base = block;

    } else {

        block_meta *last = base;

        block = NULL;
        //block = find_free_block(&last, s);
        
        if (!block) { 

            block = request_space(last, s);
            if (!block) {
                return NULL; //no more ram
            }
        } else {
            block->free = 0;
        }
    }
    return (block + 1); //return addres data of user (1 = metasize)
}

void my_free(void *ptr) {
    // TODO: Marcar el bloque como libre.
    // TODO: Fusionar bloques adyacentes (Coalescing).
}

void *my_calloc(size_t nmemb, size_t size) {
    // TODO: Usar my_malloc y luego memset a 0.
    return NULL;
}

void *my_realloc(void *ptr, size_t size) {
    // TODO: Redimensionar el bloque o moverlo a uno nuevo.
    return NULL;
}
