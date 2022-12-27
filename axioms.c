#include <string.h>
#include "data.h"
#include "pointer.h"
#include "trace.h"

void *
atom (void *x)
{
  int type = __get_tag (x);
  if (type == LISP_ATOM)
    {
      return create_atom ("t");
    }

  if (!__get_pointer (x))
    {
      return create_atom ("t");
    }

  return __create_tagged_pointer (NULL,
                                  LISP_LIST); /* return NIL or () */
}

void *
car (void *ptr)
{
  if (__get_pointer (atom (ptr)))
    {
      ptr = __get_pointer (ptr);
      char *sym = ptr ? (char *) ptr : "NIL";
      TRACE_LANGUAGE_ERROR ("%s is not a list", sym);
      return __create_tagged_pointer (NULL, LISP_LIST);
    }

  return ((lisp_node *) __get_pointer (ptr))->data;
}

void *
cdr (void *ptr)
{
  if (__get_pointer (atom (ptr)))
    {
      ptr = __get_pointer (ptr);
      char *sym = ptr ? (char *) ptr : "NIL";
      TRACE_LANGUAGE_ERROR ("%s is not a list", sym);
      return __create_tagged_pointer (NULL, LISP_LIST);
    }

  return ((lisp_node *) __get_pointer (ptr))->next;
}

void *
eq (void *x, void *y)
{
  if (x == NIL)
    {
      if (y == NIL)
        return create_atom ("t");

      return NIL;
    }

  if (__get_pointer (atom (x)) && __get_pointer (atom (y)))
    {
      x = __get_pointer (x);
      y = __get_pointer (y);

      return !strcmp (x, y)
               ? create_atom ("t")
               : __create_tagged_pointer (NULL, LISP_LIST);
    }
  return __create_tagged_pointer (NULL, LISP_LIST);
}

void *
cons (void *x, void *y)
{
  /*  if (__get_pointer(atom(y)) && __get_pointer(y)) */
  /*   { */
  /*     TRACE_LANGUAGE_ERROR("Invalid parameter: y should be LIST or
   * NIL"); */
  /*     return __create_tagged_pointer(NULL, LISP_LIST); */
  /*   } */
  /* x = create_list_node (x); */

  /* lisp_node *n = __get_pointer (x); */
  /* for (; __get_pointer(n->next)!= NULL; n = __get_pointer(cdr(x)));
   */

  /* if (__get_pointer(y)) */
  /*   { */
  /*     n->next = y; */
  /*   } */

  lisp_node *n = create_list_node (x);
  ((lisp_node *) __get_pointer (n))->next = y;
  return n;
}

void *
quote (void *x)
{
  return x;
}

void *
cadr (void *x)
{
  return (car (cdr (x)));
}

void *
caddr (void *x)
{
  return (car (cdr (cdr (x))));
}

void *
cdar (void *x)
{
  return (cdr (car (x)));
}

void *
clist (void *x)
{
  if (x == NIL)
    return NIL;

  return cons (car (x), clist (cdr (x)));
}

void *
null (void *x)
{
  return (eq (x, NIL));
}

void *
append (void *x, void *y)
{
  if (x == NIL)
    return y;

  return (cons (car (x), (append (cdr (x), y))));
}

void *
concat (void *x, void *y)
{
  if (x == NIL)
    return y;

  if (cdr (x) == NIL)
    ((lisp_node *) __get_pointer (x))->next = y;

  else
    ((lisp_node *) __get_pointer (x))->next = concat (cdr (x), y);

  return x;
}

void *
copy (void *x)
{
  if (__get_tag (x) == LISP_ATOM)
    return create_list_node (create_atom (__get_pointer (x)));

  return concat (copy (car (x)), copy (cdr (x)));
}
void *
pair (void *x, void *y)
{
  if (x == NIL && y == NIL)
    return NIL;

  if (__get_tag (x) != LISP_ATOM && __get_tag (y) != LISP_ATOM)
    return cons (cons (car (x), car (y)), pair (cdr (x), cdr (y)));
}
