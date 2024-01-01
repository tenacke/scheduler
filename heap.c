#include <stdlib.h>
#include <string.h>

#include "heap.h"

void swap(void** heap, int i, int j) {
    void* temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

int percolate_up(heap_t* heap, int i) {
    while (i > 0 && (*heap->compare_to)((heap->heap)[i], (heap->heap)[(i - 1) / 2]) < 0) {
        swap(heap->heap, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
    return i;
}

int percolate_down(heap_t* heap, int i) {
    while (2 * i + 1 < heap->heap_size) {
        int j = 2 * i + 1;
        if (j + 1 < heap->heap_size && (*heap->compare_to)((heap->heap)[j + 1], (heap->heap)[j]) < 0) {
            j++;
        }
        if ((*heap->compare_to)((heap->heap)[i], (heap->heap)[j]) < 0) {
            break;
        }
        swap(heap->heap, i, j);
        i = j;
    }
    return i;
}

heap_t* init_heap(int table_size, int (*compare_to)(void*, void*)) {
    heap_t* heap = malloc(sizeof(heap_t));
    heap->heap = calloc(table_size, sizeof(void*));
    heap->heap_size = 0;
    heap->compare_to = compare_to;
    return heap;
}

void push(heap_t* heap, void* value) {
    (heap->heap)[heap->heap_size] = value;
    heap->heap_size++;
    int i = heap->heap_size - 1;
    percolate_up(heap, i);
}

void* seek(heap_t* heap) {
    return (heap->heap)[0];
}

void* poll(heap_t* heap) {
    void* result = (heap->heap)[0];
    heap->heap_size--;
    heap->heap[0] = (heap->heap)[heap->heap_size];
    percolate_down(heap, 0);
    return result;
}

void relocate(heap_t* heap, void* value) {
    int i = 0;
    while (i < heap->heap_size && (heap->heap)[i] != value) {
        i++;
    }
    i = percolate_down(heap, i);
    percolate_up(heap, i);
}

void free_heap(heap_t* heap) {
    free(heap->heap);
    free(heap);
}
