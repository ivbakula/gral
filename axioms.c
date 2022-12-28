#include <string.h>
#include "data.h"
#include "pointer.h"
#include "trace.h"

char *PRINT (void *data);

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
      //      TRACE_LANGUAGE_ERROR ("%s is not a list", sym);
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
      //      TRACE_LANGUAGE_ERROR ("%s is not a list", sym);
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
append (void *x, void *y)
{
  if (x == NIL)
    return y;

  return (cons (car (x), (append (cdr (x), y))));
}

void *
pair (void *x, void *y)
{
  if (x == NIL && y == NIL)
    return NIL;

  if (__get_tag (x) != LISP_ATOM && __get_tag (y) != LISP_ATOM)
    return cons (cons (car (x), cons (car (y), NIL)),
                 pair (cdr (x), cdr (y)));

  return NIL;
}

void *
setq (void *x, void *y)
{
  if (x == NIL)
    return NIL;

  if (__get_tag (x) != LISP_ATOM)
    {
      TRACE_LANGUAGE_ERROR ("%s is not a symbol", PRINT (x));
      return NIL;
    }

  return pair (cons (x, NIL), cons (y, NIL));
}

void *
assoc (void *x, void *y)
{
  if (y == NIL)
    return NIL;

  if (eq (car (car (y)), x) != NIL)
    return car (cdr (car (y)));

  return assoc (x, cdr (y));
}
