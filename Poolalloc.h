// MIT License
// Copyright (c) 2026 Arran Stevens

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once
#include <stdlib.h>
#include <stdint.h>

typedef union PoolChunk PoolChunk;
union PoolChunk {
    PoolChunk* next;
    char buffer[128];
};

typedef struct ArrayStart ArrayStart;
typedef struct ArrayStart
{
    PoolChunk* array;
    ArrayStart* next;
} ArrayStart;

typedef struct PoolAlloc
{
    PoolChunk* freeChunk;
    ArrayStart* arrayStart;
    uint32_t chunksPerArray;
    uint32_t itemSize;
} PoolAlloc;

int PoolAlloc_Init(PoolAlloc* pool, uint32_t chunkCount, uint32_t itemSize)
{
    if (!pool) return 0;
    if (chunkCount < 8) chunkCount = 8;

    PoolChunk* array = (PoolChunk*)malloc(chunkCount * sizeof(PoolChunk));
    if (!array) return 0;

    ArrayStart* node = (ArrayStart*)malloc(sizeof(ArrayStart));
    if (!node) {
        free(array);
        return 0;
    }

    for (uint32_t i = 0; i < chunkCount - 1; i++) {
        array[i].next = &array[i + 1];
    }
    array[chunkCount - 1].next = NULL;

    pool->freeChunk       = array;
    pool->arrayStart      = node;
    pool->chunksPerArray  = chunkCount;

    node->array = array;
    node->next  = NULL;
    return 1;
}

int PoolAlloc_Expand(PoolAlloc* pool, uint32_t extraChunks)
{
    if (pool == NULL) return 0;

    PoolChunk* extraArray = (PoolChunk*)malloc(extraChunks * sizeof(PoolChunk));
    if (extraArray == NULL) return 0;

    ArrayStart* arrayStart = (ArrayStart*)malloc(sizeof(ArrayStart));
    if (arrayStart == NULL) {
        free(extraArray);
        return 0;
    }

    for (uint32_t i=0; i<extraChunks-1; i++) {
        extraArray[i].next = &extraArray[i+1];
    }

    extraArray[extraChunks-1].next = pool->freeChunk;
    pool->freeChunk = extraArray;

    arrayStart->array = extraArray;
    arrayStart->next = pool->arrayStart;
    pool->arrayStart = arrayStart;
    return 1;
}

void* PoolAlloc_Alloc(PoolAlloc* pool) {
    if (pool == NULL) return NULL;

    if (pool->freeChunk == NULL) {
        if (!PoolAlloc_Expand(pool, pool->chunksPerArray)) return NULL;
    }

    PoolChunk* result = pool->freeChunk;
    pool->freeChunk = pool->freeChunk->next;
    return result;
}

void PoolAlloc_Free(PoolAlloc* pool, void* ptr) {
    if (pool == NULL || ptr == NULL) return;
    PoolChunk* freed = (PoolChunk*)ptr;
    freed->next = pool->freeChunk;
    pool->freeChunk = freed;
}

void PoolAlloc_Destroy(PoolAlloc* pool) {
    if (pool == NULL) return;

    ArrayStart* arrayStart = pool->arrayStart;
    while (arrayStart != NULL) {
        ArrayStart* next = arrayStart->next;
        free(arrayStart->array);
        free(arrayStart);
        arrayStart = next;
    }

    pool->arrayStart = NULL;
    pool->freeChunk = NULL;
}
