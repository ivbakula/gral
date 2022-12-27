#ifndef __DATA_H
#define __DATA_H

#include "pointer.h"

#define LISP_ATOM 1
#define LISP_LIST 2

#define NIL __create_tagged_pointer (NULL, LISP_LIST)
typedef struct lisp_node
{
  struct lisp_node *next;
  void *data;
} lisp_node;

void *create_list_node (void *data);
void *create_atom (const char *data);

#endif
