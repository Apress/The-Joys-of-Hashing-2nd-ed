
#include "stack.h"

#include <assert.h>

int
main()
{
  struct stack *stack = new_stack();

  for (int i = 0; i < 10; ++i) {
    push(stack, i);
  }
  for (int i = 9; i >= 0; --i) {
    assert(pop(stack) == i);
  }

  free_stack(stack);

  return 0;
}
