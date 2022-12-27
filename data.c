#include "data.h"
#include <stdlib.h>
#include "pointer.h"

void *
create_list_node (void *data)
{
  lisp_node *n = (lisp_node *) malloc (sizeof (*n));
  n->data = data;
  n->next = __create_tagged_pointer (NULL, LISP_LIST);

  return __create_tagged_pointer (n, LISP_LIST);
}

void *
create_atom (const char *data)
{
  return __tagged_strdup (data, LISP_ATOM);
}
