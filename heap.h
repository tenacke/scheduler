#ifndef HEAP_H
#define HEAP_H

typedef struct {
    void** heap;
    size_t element_size;
    int heap_size;
    int (*compare_to)(void*, void*);
} heap_t;


heap_t* init_heap(int table_size, size_t element_size, int (*compare_to)(void*, void*));
void push(heap_t* heap, void* value);
void* seek(heap_t* heap);
void* poll(heap_t* heap);
void relocate(heap_t* heap, void* value);
void free_heap(heap_t* heap);

#endif