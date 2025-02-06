#include "stack.h"
#include "raylib.h"
#include "stddef.h"

Stack *StackNew()
{
  Stack *newStack = (Stack*) MemAlloc(sizeof(Stack));
  *newStack = (Stack){0, NULL, NULL};
  return newStack;
}

StackSNode *StackNewNode(void *data)
{
  StackSNode *newNode = (StackSNode*) MemAlloc(sizeof(StackSNode));
  *newNode = (StackSNode){data, NULL};
  return newNode;
}

void StackPushBack(Stack* stack, void* data){
  StackSNode* newNode = StackNewNode(data);
  if (StackIsEmpty(stack)){
    stack->head = newNode;
    stack->tail = newNode;
    stack->size = 1;
    return;
  }
  newNode->next = stack->head;
  stack->head = newNode;
  
  stack->size++;
}

void StackPopBack(Stack* stack){
  if (StackIsEmpty(stack)){
    return;
  }

  if (stack->head == stack->tail){
    MemFree(stack->head->data);
    MemFree(stack->head);
    stack->head = NULL;
    stack->tail = NULL;
    stack->size = 0;
    return;
  }

  stack->head = stack->head->next;
  stack->size--;
}

void *StackPeek(Stack *stack)
{
  if (StackIsEmpty(stack))
  {
    return NULL;
  }

  return stack->head->data;
}

int StackIsEmpty(const Stack *stack)
{
  return stack->head == NULL && stack->tail == NULL;
}

void StackClear(Stack *stack)
{
  StackSNode *current = stack->head;
  while (current != NULL)
  {
    StackSNode *temp = current;
    current = current->next;
    MemFree(temp->data);
    MemFree(temp);
  }
  stack->head = NULL;
  stack->tail = NULL;
  stack->size = 0;
}
