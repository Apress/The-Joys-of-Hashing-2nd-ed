
#include "linked_lists.h"

#include <assert.h>
#include <stdio.h>

static void
test_list(LIST list)
{
  unsigned int some_keys[] = {
      1, 2, 3, 4, 5,
  };
  size_t n = sizeof(some_keys) / sizeof(*some_keys);

  for (unsigned int i = 0; i < n; i++) {
    printf("inserting key %u\n", some_keys[i]);
    add_element(list, some_keys[i]);
  }
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in list? %d\n", some_keys[i],
           contains_element(list, some_keys[i]));

    assert(contains_element(list, some_keys[i]));
  }
  printf("\n");

  printf("Removing keys 3 and 4\n");
  delete_element(list, 3);
  delete_element(list, 4);
  printf("\n");

  for (unsigned int i = 0; i < n; i++) {
    printf("is key %u in list? %d\n", some_keys[i],
           contains_element(list, some_keys[i]));
    if (some_keys[i] == 3 || some_keys[i] == 4)
      assert(!contains_element(list, some_keys[i]));
    else
      assert(contains_element(list, some_keys[i]));
  }
  printf("\n");
}

int
main()
{
  LIST static_list = EMPTY_LIST;
  test_list(static_list);
  free_list(static_list);

  LIST owned_list = new_owned_list();
  test_list(owned_list);
  free_owned_list(owned_list);

  return 0;
}
