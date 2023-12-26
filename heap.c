#include <stdlib.h>
#include <string.h>

#include "heap.h"

void swap(void* a, void* b, size_t size) {
    void* temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}

heap_t* init_heap(int table_size, size_t element_size, int (*compare_to)(void*, void*)) {
    heap_t* heap = malloc(sizeof(heap_t));
    heap->heap = calloc(table_size, sizeof(void*));
    heap->element_size = element_size;
    heap->heap_size = 0;
    heap->compare_to = compare_to;
    return heap;
}

void push(heap_t* heap, void* value) {
    (heap->heap)[heap->heap_size] = value;
    heap->heap_size++;
    int i = heap->heap_size - 1;
    while (i > 0 && (*heap->compare_to)((heap->heap)[i], (heap->heap)[(i - 1) / 2]) < 0) {
        swap((heap->heap)[i], (heap->heap)[(i - 1) / 2], heap->element_size);
        i = (i - 1) / 2;
    }
}

void* seek(heap_t* heap) {
    return (heap->heap)[0];
}

void* poll(heap_t* heap) {
    void* result = (heap->heap)[0];
    heap->heap_size--;
    heap->heap[0] = (heap->heap)[heap->heap_size];
    int i = 0;
    while (2 * i + 1 < heap->heap_size) {
        int j = 2 * i + 1;
        if (j + 1 < heap->heap_size && (*heap->compare_to)((heap->heap)[j + 1], (heap->heap)[j]) < 0) {
            j++;
        }
        if ((*heap->compare_to)((heap->heap)[i], (heap->heap)[j]) < 0) {
            break;
        }
        swap((heap->heap)[i], (heap->heap)[j], heap->element_size);
        i = j;
    }
    return result;
}

void free_heap(heap_t* heap) {
    free(heap->heap);
    free(heap);
}
