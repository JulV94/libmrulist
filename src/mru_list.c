#include "mru_list.h"

mruList_s* initList(unsigned int maxSize)
{
    mruList_s *lst = (mruList_s*)malloc(sizeof(mruList_s));
    if (lst == NULL)
    {
        perror("Unable to allocate the list in memory");
        exit(EXIT_FAILURE);
    }
    lst->start = NULL;
    lst->end = NULL;
    lst->size = 0;
    lst->maxSize = maxSize;
    setMruCompare(lst, mruStdCompare);
    setMruCopy(lst, mruStdCopy);
    setMruAllocate(lst, mruStdAllocate);
    setMruFree(lst, mruStdFree);
    pthread_mutex_init(&(lst->mruMutex), NULL);
    return lst;
}

void freeList(mruList_s* mruList)
{
    pthread_mutex_lock(&(mruList->mruMutex));
    if (mruList->size > 0)
    {
        node_s* node = mruList->end->next;
        while (node != NULL)
        {
            mruList->functions.free(node->prev->data);
            free(node->prev);
            node = node->next;
        }
        free(mruList->start);
    }
    pthread_mutex_unlock(&(mruList->mruMutex));
    pthread_mutex_destroy(&(mruList->mruMutex));
    free(mruList);
}

void changeListHead(mruList_s* mruList, node_s* newHead)
{
    if (newHead != mruList->start)
    {
        newHead->next = NULL;
        newHead->prev = mruList->start;
        if (mruList->start != NULL)
        {
            mruList->start->next = newHead;
        }
        mruList->start = newHead;
    }
}

void put(mruList_s* mruList, void* data)
{
    if (data == NULL)
    {
        fprintf(stderr, "Trying to put NULL data in list, skipping...\n");
    }
    else
    {
        unsigned int index = isInList(mruList, data);
        if (index)  // means it's already in the list
        {
            get(mruList, index-1);
        }
        else
        {
            pthread_mutex_lock(&(mruList->mruMutex));
            node_s *newNode = (node_s*)malloc(sizeof(node_s));
            if (newNode == NULL)
            {
                perror("Unable to allocate the new node in list");
                exit(EXIT_FAILURE);
            }
            newNode->data = data;
            changeListHead(mruList, newNode);

            if (mruList->size == 0)
            {
                mruList->end = newNode;
            }
            mruList->size++;

            if (mruList->size > mruList->maxSize)
            {
                // Pop the last one
                mruList->size--;
                node_s *newLast = mruList->end->next;
                free(mruList->end);
                newLast->prev = NULL;
                mruList->end = newLast;
            }
            pthread_mutex_unlock(&(mruList->mruMutex));
        }
    }
}

void* get(mruList_s* mruList, unsigned int index)
{
    if (index == 0)
    {
        return getFirst(mruList);
    }
    else if (index == mruList->size-1)
    {
        return getLast(mruList);
    }
    pthread_mutex_lock(&(mruList->mruMutex));
    node_s *wantedNode = mruList->start;
    for (unsigned int i=0;i<index;i++)
    {
        if (wantedNode->prev == NULL)
        {
            fprintf(stderr, "List index provided to get function out of bounds\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            wantedNode = wantedNode->prev;
        }
    }
    // Link the 2 neighbours nodes
    if (wantedNode->next != NULL)
    {
        wantedNode->next->prev = wantedNode->prev;
    }
    if (wantedNode->prev != NULL)
    {
        wantedNode->prev->next = wantedNode->next;
    }

    // Put the wanted node on top of the list
    changeListHead(mruList, wantedNode);

    pthread_mutex_unlock(&(mruList->mruMutex));
    return wantedNode->data;
}

void* getFirst(mruList_s* mruList)
{
    return mruList->start->data;
}

void* getLast(mruList_s* mruList)
{
    pthread_mutex_lock(&(mruList->mruMutex));
    node_s *wantedNode = mruList->end;
    if (wantedNode->next != NULL)
    {
        wantedNode->next->prev = wantedNode->prev;
    }
    mruList->end = wantedNode->next;

    // Put the wanted node on top of the list
    changeListHead(mruList, wantedNode);
    pthread_mutex_unlock(&(mruList->mruMutex));
    return wantedNode->data;
}

int isInList(mruList_s* mruList, void* data)
{
    unsigned int counter = 1;
    pthread_mutex_lock(&(mruList->mruMutex));
    if (mruList->functions.compare == NULL)
    {
        perror("The compare function is not set");
        exit(EXIT_FAILURE);
    }
    if (mruList->size > 0)
    {
        node_s* node = mruList->start;
        while (node != NULL)
        {
            if (mruList->functions.compare(node->data, data))
            {
                pthread_mutex_unlock(&(mruList->mruMutex));
                return counter;
            }
            counter++;
            node = node->prev;
        }
    }
    pthread_mutex_unlock(&(mruList->mruMutex));
    return 0;
}

void* look(mruList_s* mruList, unsigned int index)
{
    pthread_mutex_lock(&(mruList->mruMutex));
    node_s *wantedNode = mruList->start;
    for (unsigned int i=0;i<index;i++)
    {
        if (wantedNode->prev == NULL)
        {
            fprintf(stderr, "List index provided to get function out of bounds\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            wantedNode = wantedNode->prev;
        }
    }
    pthread_mutex_unlock(&(mruList->mruMutex));
    return wantedNode->data;
}

void setMruCompare(mruList_s* mruList, int (*compare)(void*, void*))
{
    if (compare == NULL)
    {
        fprintf(stderr, "Cannot set a NULL pointer function to compare");
    }
    else
    {
        mruList->functions.compare = compare;
    }
}

void setMruCopy(mruList_s* mruList, int (*copy)(void*, void*))
{
    if (copy == NULL)
    {
        fprintf(stderr, "Cannot set a NULL pointer function to copy");
    }
    else
    {
        mruList->functions.copy = copy;
    }
}

void setMruAllocate(mruList_s* mruList, void* (*allocate)())
{
    if (allocate == NULL)
    {
        fprintf(stderr, "Cannot set a NULL pointer function to allocate");
    }
    else
    {
        mruList->functions.allocate = allocate;
    }
}

void setMruFree(mruList_s* mruList, void (*free)(void*))
{
    if (free == NULL)
    {
        fprintf(stderr, "Cannot set a NULL pointer function to free");
    }
    else
    {
        mruList->functions.free = free;
    }
}

int mruStdCompare(void* data1, void* data2)
{
    return data1 == data2;
}

int mruStdCopy(void* src, void* dst)
{
    // No copy method set
    fprintf(stderr, "No copy method set for the mru list only 1 byte will be copied");
    memcpy(dst, src, sizeof(char)); // Dummy code to avoid the blocking warnings
    return -1;
}

void* mruStdAllocate()
{
    return NULL; // Dummy code to avoid the blocking warnings
    // If there is some malloc to do
}

void mruStdFree(void* data)
{
    if (data){} // Dummy code to avoid the blocking warnings
    // If there was some malloc in the data send that needs
    // to be freed when freeing the list
}
