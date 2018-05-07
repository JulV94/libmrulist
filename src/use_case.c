#include <stdio.h>
#include <stdlib.h>
#include "mru_list.h"

#define STR_LEN 10

typedef struct test_s{
  int32_t intData;
  char str[STR_LEN];
} test_s;

void seek_to_next_line( void )
{
    int c;
    while( (c = fgetc( stdin )) != EOF && c != '\n' );
}

void* allocateItem()
{
  void *item = malloc(sizeof(test_s));
  if (item == NULL)
  {
    printf("Cannot allocate an item in memory");
    exit(EXIT_FAILURE);
  }
  return item;
}

void freeItem(void* item)
{
  free(item);
}

test_s* constructData(mruList_s* lst)
{
  test_s* item = (test_s*)lst->functions.allocate();
  printf("Enter the 32-bits integer of the item :");
  scanf("%d", &item->intData);
  seek_to_next_line();
  printf("Enter the string (max %d char long) :", STR_LEN-1);
  fgets(item->str, sizeof(item->str), stdin);
  return item;
}

void addItem(mruList_s* lst)
{
  put(lst, constructData(lst));
}

void getItem(mruList_s* lst)
{
  int index;
  printf("Enter index of the item to get :");
  scanf("%d", &index);
  seek_to_next_line();
  test_s *item = (test_s*)get(lst, index);
  printf("Item at index %d contains\nintData\t%d\nstr\t%s\n", index, item->intData, item->str);
}

void printList(mruList_s* lst)
{
  test_s *item;
  for (int i=0;i<lst->size;i++)
  {
    item = (test_s*)look(lst, i);
    printf("%d\t%d\t%s\n", i, item->intData, item->str);
  }
}

void main()
{
  int size;
  char entry = 'a';
  int index;
  mruList_s *lst1, *lst2;
  printf("Example of use of the mru_list library\n\n");
  printf("Enter max size of the mru lists :");
  scanf("%d", &size);
  seek_to_next_line();
  printf("Creating the 2 mru lists...\n");
  lst1 = initList(size);
  setMruFree(lst1, freeItem);
  setMruFree(lst2, freeItem);
  setMruAllocate(lst1, allocateItem);
  setMruAllocate(lst2, allocateItem);
  lst2 = initList(size);
  while (entry != 'e')
  {
    printf("Press a to add an item to list 1\nPress g to get an item from list 1\nPress b to add an item to list 2\nPress h to get an item from list 2\nPress e to exit\n>");
    scanf("%c", &entry);
    seek_to_next_line();
    switch (entry)
    {
      case 'a':
      addItem(lst1);
      break;
      case 'b':
      addItem(lst2);
      break;
      case 'g':
      getItem(lst1);
      break;
      case 'h':
      getItem(lst2);
      break;
      case 'e':
      printf("Exiting...\n");
      break;
      default:
      printf("Invalid key\n");
      break;
    }
    printf("Content of list 1\n");
    printList(lst1);
    printf("Content of list 2\n");
    printList(lst2);
  }

  freeList(lst1);
  freeList(lst2);
}
