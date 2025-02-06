#include "doubly_linked_list.h"
#include "stdlib.h"
#include "raylib.h"
#include "string.h"

DoublyLinkedList *DListNew()
{
  DoublyLinkedList *newList = MemAlloc(sizeof(DoublyLinkedList));
  *newList = (DoublyLinkedList){0, NULL, NULL};
  return newList;
}

DNode *DListNewNode(void *data)
{
  DNode *newNode = MemAlloc(sizeof(DNode));
  *newNode = (DNode){data, NULL, NULL};
  return newNode;
}

void PushFront(DoublyLinkedList *list, void *data)
{
  DNode *newNode = DListNewNode(data);
  if (DListIsEmpty(list))
  {
    list->head = newNode;
    list->tail = newNode;
    list->size = 1;
    return;
  }

  list->head->previous = newNode;
  newNode->next = list->head;
  list->head = newNode;

  list->size++;
}

void DListPushBack(DoublyLinkedList *list, void *data)
{
  DNode *newNode = DListNewNode(data);
  if (DListIsEmpty(list))
  {
    list->head = newNode;
    list->tail = newNode;
    list->size = 1;
    return;
  }

  list->tail->next = newNode;
  newNode->previous = list->tail;
  list->tail = newNode;

  list->size++;
}

void PopFront(DoublyLinkedList *list)
{
  if (DListIsEmpty(list))
  {
    return;
  }

  DNode *headTemp = list->head;

  if (list->head == list->tail)
  {
    list->head = NULL;
    list->tail = NULL;
  }
  else
  {
    list->head = list->head->next;
    list->head->previous = NULL;
  }

  MemFree(headTemp->data);
  MemFree(headTemp);
  list->size--;
}

void PopBack(DoublyLinkedList *list)
{
  if (DListIsEmpty(list))
  {
    return;
  }

  DNode *tailTemp = list->head;

  if (list->head == list->tail)
  {
    list->head = NULL;
    list->tail = NULL;
  }
  else
  {
    list->tail = list->tail->previous;
    list->tail->next = NULL;
  }

  MemFree(tailTemp->data);
  MemFree(tailTemp);
  list->size--;
}

void DListRemoveNode(DoublyLinkedList *list, DNode *node)
{
  if (DListIsEmpty(list) || node == NULL)
  {
    return;
  }

  if (list->head == list->tail)
  {
    if (node == list->head)
    {
      list->head = NULL;
      list->tail = NULL;
      MemFree(node->data);
      MemFree(node);
      list->size = 0;
      return;
    }
  }
  else
  {
    if (node->previous)
    {
      node->previous->next = node->next;
    }
    if (node->next)
    {
      node->next->previous = node->previous;
    }
    if (list->head == node)
    {
      list->head = node->next;
    }
    if (list->tail == node)
    {
      list->tail = node->previous;
    }
    MemFree(node->data);
    MemFree(node);
    list->size--;
  }
}

int DListIsEmpty(const DoublyLinkedList *list)
{
  return (list->head == NULL && list->tail == NULL);
}

void *DListToArray(DoublyLinkedList *list, size_t elementSize, int *outSize)
{
  if (DListIsEmpty(list))
  {
    return NULL;
  }

  void **array = MemAlloc(list->size * elementSize);
  if (!array)
    return NULL;

  *outSize = (int)list->size;

  DNode *current = list->head;
  size_t index = 0;
  while (current)
  {
    memcpy((char *)array + index * elementSize, current->data, elementSize);
    current = current->next;
    index++;
  }

  return array;
}

void DListClear(DoublyLinkedList *list)
{
  DNode *current = list->head;
  while (current)
  {
    DNode *temp = current;
    current = current->next;
    MemFree(temp->data);
    MemFree(temp);
  }
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}
