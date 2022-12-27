#ifndef __POINTER_H
#define __POINTER_H

#include <stddef.h>

#define TAG_BITS 2
#define PTR_BITS 64 - TAG_BITS
#define PTR_MAX (unsigned long) ((1 << PTR_BITS) - 1)

typedef union
{
  struct
  {
    unsigned long _ptr : PTR_BITS;
    unsigned long _tag : TAG_BITS;
  };
  void *ptr;
} __tagged_ptr;

unsigned long __get_tag (void *ptr);

void *__get_pointer (void *ptr);

void *__create_tagged_pointer (void *ptr, unsigned short tag);

void *__tagged_malloc (size_t, unsigned char type);

void *__tagged_strdup (const char *str, unsigned char type);

#endif
