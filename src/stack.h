#ifndef STACK_H_
#define STACK_H_

// Stack's back is the head of the linked list
typedef struct StackSNode {
  void* data;
  struct StackSNode* next;
} StackSNode;

typedef struct Stack {
  int size;
  StackSNode* head;
  StackSNode* tail;
} Stack;

Stack* StackNew();  
StackSNode* StackNewNode(void* data);
void StackPushBack(Stack* stack, void* data);
void StackPopBack(Stack* stack);
void* StackTop(Stack* stack);
int StackIsEmpty(const Stack* stack);
void StackClear(Stack* stack);

#endif