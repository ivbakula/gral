#include <stdlib.h>
#include <string.h>

#include "pointer.h"
#include "trace.h"

unsigned long
__get_tag (void *ptr)
{
  __tagged_ptr t = { .ptr = ptr };
  return t._tag;
}

void *
__get_pointer (void *ptr)
{
  void *test = ptr;
  __tagged_ptr t = { .ptr = ptr };

  return (void *) t._ptr;
  //  return (void *)((__tagged_ptr) ptr)._ptr;
}

void *
__create_tagged_pointer (void *ptr, unsigned short tag)
{
  __tagged_ptr t = { ._ptr = (unsigned long) ptr, ._tag = tag };

  return t.ptr;
}

void *
__tagged_malloc (size_t size, unsigned char type)
{
  __tagged_ptr t
    = { ._ptr = (unsigned long) malloc (size), ._tag = type };
  return t.ptr;
}

void *
__tagged_strdup (const char *str, unsigned char type)
{
  __tagged_ptr t
    = { ._ptr = (unsigned long) strdup (str), ._tag = type };

  return t.ptr;
}
