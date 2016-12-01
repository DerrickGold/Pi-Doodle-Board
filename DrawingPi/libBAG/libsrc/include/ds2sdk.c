#include "ds2sdk.h"


void *bag_malloc(unsigned long size){
    ALIGN_MEM(size)
    return malloc(size);
}

void *bag_calloc(unsigned long count, unsigned long size){
    ALIGN_MEM(count)
    return calloc(count, size);
}

void *bag_realloc(void *ptr, unsigned long size){
    if(ptr != NULL){
        free(ptr);
    }
    return calloc(size, sizeof(char));
}
