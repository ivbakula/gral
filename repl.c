#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "axioms.h"
#include "data.h"
#include "pointer.h"
#include "trace.h"

static char output_buffer[1024];

static void
lisp_putchar (char c)
{
  char str[2] = { c, 0 };
  strcat (output_buffer, str);
}

static void
lisp_putstring (char *c)
{
  strcat (output_buffer, c);
}

static void
lisp_clear_buffer ()
{
  memset (output_buffer, '\0', 1024);
}

static inline int
is_whitespace (char c)
{
  return (c == ' ' || c == '\t' || c == '\n');
}

static inline int
is_printable (char c)
{
  return (c > 32 && c < 40)
         || (c > 41 && c < 127); // every character that is not
                                 // whitespace or '(' ')'
}

static char *
read_atom (char **line)
{
  char *start = *line;
  int offset = 0;
  while (is_printable (*(start + offset)))
    offset++;

  int strsize = offset + 1;
  char *str = (char *) (malloc (strsize * sizeof (*str)));
  memset (str, '\0', strsize * sizeof (*str));
  memcpy (str, start, strsize - 1);
  str[strsize] = 0; // null terminate string

  *line += offset;
  return __create_tagged_pointer (str, LISP_ATOM);
}

static void
eat_whitespace (char **line)
{
  char *start = *line;
  int offset = 0;

  while (is_whitespace (*(start + offset)))
    offset++;

  *line += offset;
}

void *
read_list (char **line, int level)
{
  void *list = NIL;

  while (**line)
    {
      switch (**line)
        {
        case '(':
          *line += 1;
          list = append (list, cons (read_list (line, ++level), NIL));
          break;
        case ')':
          *line += 1;
          return list;
        default:
          if (is_whitespace (**line))
            eat_whitespace (line);
          else if (is_printable (**line))
            list = append (list, cons (read_atom (line), NIL));
          break;
        }
    }
  return list;
}

void *
READ (char *line)
{
  char *l = line;
  if (is_printable (*l))
    return read_atom (&l);

  void *ret = read_list (&l, 0);
  return ((ret != NIL) ? car (ret) : NIL);
}

void *
EVAL (void *list, void *env)
{
  if (eq (atom (list), create_atom ("t")) != NIL)
    {
      return list;
    }
  if (atom (car (list)) != NIL)
    {
      if (eq (car (list), create_atom ("quote")) != NIL)
        {
          return cdr (list);
        }

      if (eq (car (list), create_atom ("atom")) != NIL)
        {
          return atom (EVAL (cadr (list), env));
        }

      if (eq (car (list), create_atom ("eq")) != NIL)
        {
          return eq (EVAL (cadr (list), env),
                     EVAL (caddr (list), env));
        }

      if (eq (car (list), create_atom ("car")) != NIL)
        {
          return car (EVAL (cadr (list), env));
        }

      if (eq (car (list), create_atom ("cdr")) != NIL)
        {
          return cdr (EVAL (cadr (list), env));
        }

      if (eq (car (list), create_atom ("cons")) != NIL)
        {
          return cons (EVAL (cadr (list), env),
                       EVAL (caddr (list), env));
        }
    }
  return list;
}

static void
internal_print (void *ptr)
{
  if (ptr == NIL)
    lisp_putstring ("()");

  else
    lisp_putstring ((char *) __get_pointer (ptr));
}

static void
print_list (void *data)
{
  lisp_putstring ("(");
  for (void *list = data; list != NIL; list = cdr (list))
    {
      if (__get_tag (car (list)) == LISP_ATOM)
        {
          internal_print (car (list));
          if (cdr (list) != NIL)
            lisp_putstring (" ");
        }
      else if (atom (car (list)) == NIL)
        {
          print_list (car (list));
          if (cdr (list) != NIL)
            lisp_putstring (" ");
        }
    }
  lisp_putstring (")");
}

char *
PRINT (void *data)
{
  lisp_clear_buffer ();
  if (data == NIL || __get_tag (data) == LISP_ATOM)
    internal_print (data);
  else
    print_list (data);

  return strdup (output_buffer);
}
