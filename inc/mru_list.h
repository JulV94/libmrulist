#ifndef MRU_LIST_INCLUDED
#define MRU_LIST_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

typedef struct node_s {
    struct node_s* next;
    struct node_s* prev;
    void* data;
} node_s;

typedef struct mruFct_s {
    int (*compare)(void*, void*);
    int (*copy)(void*, void*);
    void* (*allocate)();
    void (*free)(void*);
} mruFct_s;

typedef struct mruList_s {
    node_s* start;
    node_s* end;
    unsigned int size;
    unsigned int maxSize;
    mruFct_s functions;
    pthread_mutex_t mruMutex;
} mruList_s;

// Init the list
mruList_s* initList(unsigned int maxSize);

// Free the list
void freeList(mruList_s* mruList);

// Change the first item in the list
void changeListHead(mruList_s* mruList, node_s* newHead);

// Add an item to the list
void put(mruList_s* mruList, void* data);

// Get an item in the list by index
void* get(mruList_s* mruList, unsigned int index);

// Get first item
void* getFirst(mruList_s* mruList);

// Get last item
void* getLast(mruList_s* mruList);

// Is the item in list (return 0 if not or index+1 if it's in it)
int isInList(mruList_s* mruList, void* data);

// Returns data without changing list order
void* look(mruList_s* mruList, unsigned int index);

// Set compare function
void setMruCompare(mruList_s* mruList, int (*compare)(void*, void*));

// Set copy function
void setMruCopy(mruList_s* mruList, int (*copy)(void*, void*));

// Set allocate function
void setMruAllocate(mruList_s* mruList, void* (*allocate)());

// Set free function
void setMruFree(mruList_s* mruList, void (*free)(void*));

// Default compare function
int mruStdCompare(void* data1, void* data2);

// Default copy function
int mruStdCopy(void* srd, void* dst);

// Default allocate function
void* mruStdAllocate();

// Default free function
void mruStdFree(void* data);

#endif
