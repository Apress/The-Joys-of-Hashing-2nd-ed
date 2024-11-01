
#include "stack.h"

#include <assert.h>
#include <stdlib.h>

struct stack *
new_stack()
{
  struct stack *stack = malloc(sizeof *stack);
  *stack = (struct stack){.size = 1,
                          .used = 0,
                          .array = malloc(sizeof *stack->array)};
  return stack;
}

void
free_stack(struct stack *stack)
{
  free(stack->array);
  free(stack);
}

static void
resize(struct stack *stack, unsigned int new_size)
{
  assert(new_size >= stack->used);
  stack->array = realloc(stack->array, new_size * sizeof *stack->array);
  stack->size = new_size;
}

void
push(struct stack *stack, int value)
{
  if (stack->used == stack->size)
    resize(stack, 2 * stack->size);
  stack->array[stack->used++] = value;
}

bool
is_empty(struct stack *stack)
{
  return stack->used == 0;
}

int
pop(struct stack *stack)
{
  assert(stack->used > 0);
  int top = stack->array[--(stack->used)];
  if (stack->used < stack->size / 4)
    resize(stack, stack->size / 2);
  return top;
}
