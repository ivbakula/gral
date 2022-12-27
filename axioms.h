#ifndef __AXIOM_H
#define __AXIOM_H

void *atom (void *x);
void *car (void *ptr);
void *cdr (void *ptr);
void *eq (void *x, void *y);
void *cons (void *x, void *y);
void *quote (void *x);
void *append (void *x, void *y);
void *quote (void *x);
void *cadr (void *x);
void *caddr (void *x);
void *cdar (void *x);
void *null (void *x);
void *append (void *x, void *y);
void *pair (void *x, void *y);
void *clist (void *x);
void *concat (void *x, void *y);
void *copy (void *x);
#endif
