
#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

struct stack {
  int *array;
  unsigned int size;
  unsigned int used;
};

struct stack *
new_stack(void);
void
free_stack(struct stack *stack);

void
push(struct stack *stack, int value);
int
pop(struct stack *stack);
bool
is_empty(struct stack *stack);

#endif
