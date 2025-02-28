#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H
#include "stddef.h"

typedef struct DNode {
  void* data;
  struct DNode* next;
  struct DNode* previous;
} DNode;

typedef struct DoublyLinkedList {
  size_t size;
  DNode* head;
  DNode* tail;
} DoublyLinkedList;


DoublyLinkedList* DListNew();
DNode* DListNewNode(void* data);
void DListPushBack(DoublyLinkedList* list, void* data);
void DListRemoveNode(DoublyLinkedList *list, DNode* node);
int DListIsEmpty(const DoublyLinkedList* list);
void *DListToArray(DoublyLinkedList *list, size_t elementSize, int *outSize);
void DListClear(DoublyLinkedList *list);

#endif