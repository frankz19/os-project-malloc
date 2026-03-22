#include <unistd.h> // Para sbrk
#include "mm_malloc.h"
#include <sys/types.h>
#include <stddef.h>
#include <string.h>


// silent apple erros of sbrk deprecated
#ifdef __APPLE__
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif



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
 * find_free_block: search avaliable and sufucient block in list.
 * last: pointer to save last block visited.
 * size: size necesary.
 * @return: pointer to free block or NULL.
 * 
 * 
 * split_block: split large block to free one.
 * block: original large block.
 * size: size of necesary space.
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

static block_meta *find_free_block(block_meta **last, size_t size) {
    block_meta *current = (block_meta *)base;

    while (current && !(current->free && current->size >= size)) {
        *last = current; //update last
        current = current->next;
    }
    return current; //return valid adress block or NULL
}

static void split_block(block_meta *block, size_t size) {
    
    if (block->size >= (size + META_SIZE + 16)) { // spliting threshold

        block_meta *new_free_block = (block_meta *)((char *)block + META_SIZE + size);

        new_free_block->size = block->size - size - META_SIZE;
        new_free_block->next = block->next;
        new_free_block->free = 1;
        new_free_block->magic = 0x4652414E;

        block->size = size;
        block->next = new_free_block;
    }
}

//extra functions





void *my_malloc(size_t size) {
    block_meta *block;

    if (size > (size_t)-1 - 7 - META_SIZE) {
        return NULL; 
    }
    

    if (size <= 0) {
        return NULL;
    }

    size_t s = ALIGN(size);
    
    if (base == NULL) {

        block = request_space(NULL, s);
        
        if (!block) {
            return NULL; // kernel say no more ram
        }

        base = block;

    } else {

        block_meta *last = base;

        block = find_free_block(&last, s);
        
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
    
    if (!ptr) {
        return;
    }

    if (ptr < base) {
        return;
    }


    block_meta *block = (block_meta*)ptr -1; //acces to strcut data of block

    if ( block->magic != 0x4652414E) { //check valid block with secutity sign
        return;
    }

    if (block->free) {
        return;
    }
    
    block->free = 1;
}

void *my_calloc(size_t nmemb, size_t size) {
    
    if (size != 0 && nmemb > (size_t)-1 / size) {
        return NULL; // Overflow detectado, no podemos asignar tanta memoria
    }
    
    size_t necesary_size = nmemb * size;

    void *ptr = my_malloc(necesary_size);
    if (ptr) {
        memset(ptr, 0, necesary_size);
    }
    return ptr;
}

void *my_realloc(void *ptr, size_t size) {
    
    if (!ptr) {
        return my_malloc(size);
    }

    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    size_t s = ALIGN(size);
    block_meta *block = (block_meta *)ptr - 1;
    
    if (block->size >= s) {
        split_block(block, s);
        return ptr;
    }

    void *new_ptr = my_malloc(size);

    if(!new_ptr) {
        return NULL;
    }

    memcpy(new_ptr, ptr, block->size);

    my_free(ptr);

    return new_ptr;
    
}
