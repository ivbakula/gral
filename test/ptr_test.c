#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../axioms.h"
#include "../data.h"
#include "../pointer.h"
#include "../repl.h"
#include "../trace.h"

void
test_pointer_tagging ()
{
  LOG_START_TESTCASE ();
  const int tag = 2;
  char *string = strdup ("Hello world");
  void *ptr = __create_tagged_pointer (string, tag);

  assert (string == __get_pointer (ptr));
  assert (tag == __get_tag (ptr));
  assert (!strcmp (string, __get_pointer (ptr)));
  LOG_END_TESTCASE ();
}

void
test_non_tagged_pointer ()
{
  LOG_START_TESTCASE ();
  char *string = strdup ("Hello world");

  assert (string == __get_pointer (string));
  assert (0 == __get_tag (string));
  assert (!strcmp (__get_pointer (string), string));
  LOG_END_TESTCASE ();
}
void
test_tagged_malloc ()
{
  LOG_START_TESTCASE ();
  int type = 2;
  void *ptr = __tagged_malloc (24, type);
  assert (type == __get_tag (ptr));

  type = 1;
  ptr = __tagged_malloc (24, type);
  assert (type == __get_tag (ptr));
  LOG_END_TESTCASE ();
}

void
test_tagged_strdup ()
{
  LOG_START_TESTCASE ();
  int type = 3;
  char *str = "HELLO WORLD";
  void *ptr = __tagged_strdup (str, type);

  assert (type == __get_tag (ptr));
  assert (!strcmp (str, __get_pointer (ptr)));
  LOG_END_TESTCASE ();
}

void
test_create_atom ()
{
  LOG_START_TESTCASE ();
  const char *str = "test";
  void *a = create_atom (str);

  assert (LISP_ATOM == __get_tag (a));
  assert (!strcmp (str, __get_pointer (a)));
  LOG_END_TESTCASE ();
}

void
test_create_list_node ()
{
  LOG_START_TESTCASE ();
  const char *str = "test";
  void *a = create_atom (str);

  void *n = create_list_node (a);
  assert (LISP_LIST == __get_tag (n));
  assert ((lisp_node *) __get_pointer (n) != NULL);

  void *tst_data = ((lisp_node *) __get_pointer (n))->data;
  assert (__get_tag (tst_data) == LISP_ATOM);
  assert (__get_pointer (tst_data) == __get_pointer (a));

  void *tst_next = ((lisp_node *) __get_pointer (n))->next;
  assert (__get_tag (tst_next) == LISP_LIST);
  assert (__get_pointer (tst_next) == NULL);
  LOG_END_TESTCASE ();
}

void
test_atom ()
{
  LOG_START_TESTCASE ();
  /* test identifiers */
  void *test = create_atom ("test");
  void *ret = atom (test);

  assert (__get_tag (ret) == LISP_ATOM);
  assert (!strcmp (__get_pointer (ret), "t"));

  /* test NIL */
  ret = atom (NIL);
  assert (__get_tag (ret) == LISP_ATOM);
  assert (__get_pointer (ret));
  assert (!strcmp (__get_pointer (ret), "t"));

  /* test LIST */
  void *list = create_list_node (NIL);
  ret = atom (list);
  assert (__get_tag (ret) == LISP_LIST);
  assert (!__get_pointer (ret));
  LOG_END_TESTCASE ();
}

void
test_cons ()
{
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("(cons 'world '())");
  void *a = create_atom ("world");
  void *l = cons (a, NIL);

  assert (__get_tag (l) == LISP_LIST);
  assert (__get_pointer (l));

  lisp_node *tst_l = __get_pointer (l);
  assert (tst_l->data == a);
  assert (!strcmp (__get_pointer (a), __get_pointer (tst_l->data)));
  assert (tst_l->next == NIL);
  LOG_END_TEST_STEP ("(cons 'world '())");

  LOG_START_TEST_STEP ("(cons 'hello '(world))");
  void *hello = create_atom ("hello");
  void *ll = cons (hello, l);
  assert (__get_tag (ll) == LISP_LIST);
  assert (((lisp_node *) __get_pointer (ll))->data == hello);
  assert (((lisp_node *) __get_pointer (ll))->next == l);
  LOG_END_TEST_STEP ("(cons 'hello '(world))");
  LOG_END_TESTCASE ();
}

void
test_car ()
{
  LOG_START_TESTCASE ();
  char *hello = create_atom ("hello");
  char *world = create_atom ("world");
  void *list = cons (hello, cons (world, NIL));

  LOG_START_TEST_STEP ("(car '(hello world))");
  void *v = car (list);
  assert (__get_pointer (v));
  assert (__get_tag (v) == LISP_ATOM);
  assert (!strcmp (__get_pointer (v), "hello"));
  LOG_END_TEST_STEP ("(car '(hello world)) SUCCESSFUL");
  LOG_END_TESTCASE ();
}

void
test_cdr ()
{
  LOG_START_TESTCASE ();
  char *hello = create_atom ("Hello");
  char *test = create_atom ("test");
  char *world = create_atom ("world");
  void *list = cons (hello, cons (test, cons (world, NIL)));

  LOG_START_TEST_STEP ("cdr '(hello test world)");
  list = cdr (list);
  assert (__get_pointer (list));
  assert (__get_tag (list) == LISP_LIST);
  assert (__get_pointer (car (list)));
  assert (!strcmp (__get_pointer (car (list)), __get_pointer (test)));
  LOG_END_TEST_STEP ("cdr '(hello test world)");

  LOG_START_TEST_STEP ("cdr '(test world)");
  list = cdr (list);
  assert (__get_pointer (list) != NULL);
  assert (__get_tag (list) == LISP_LIST);
  assert (__get_pointer (car (list)) != NULL);
  assert (
    !strcmp (__get_pointer (car (list)), __get_pointer (world)));
  LOG_END_TEST_STEP ("cdr '(test world)");

  LOG_START_TEST_STEP ("cdr '(world)");
  list = cdr (list);
  assert (!__get_pointer (list));
  assert (__get_tag (list) == LISP_LIST);
  assert (!__get_pointer (car (list)));
  LOG_END_TEST_STEP ("cdr '(world)");

  LOG_START_TEST_STEP ("cdr '()");
  list = cdr (list);
  assert (!__get_pointer (list));
  assert (__get_tag (list) == LISP_LIST);
  assert (!__get_pointer (car (list)));
  LOG_END_TEST_STEP ("cdr '()");

  LOG_END_TESTCASE ();
}

void
test_cons_with_lists ()
{
  LOG_START_TESTCASE ();
  void *x
    = cons (create_atom ("a"),
            cons (create_atom ("b"), cons (create_atom ("c"), NIL)));

  void *y
    = cons (create_atom ("1"),
            cons (create_atom ("2"), cons (create_atom ("3"), NIL)));

  void *xy = cons (x, y);
  void *x0 = cons (x, NIL);

  LOG_START_TEST_STEP ("car '((a b c) 1 2 3)");
  void *cx = car (xy);
  assert (__get_pointer (cx));
  assert (__get_tag (cx) == LISP_LIST);
  assert (!strcmp (__get_pointer (car (cx)), "a"));
  assert (!strcmp (__get_pointer (car (cdr (cx))), "b"));
  assert (!strcmp (__get_pointer (car (cdr (cdr (cx)))), "c"));
  assert (__get_tag (car (cdr (cdr (cdr (cx))))) == LISP_LIST);
  assert (__get_pointer (car (cdr (cdr (cdr (cx))))) == NULL);

  void *cy = cdr (xy);
  assert (__get_pointer (cy));
  assert (__get_tag (cy) == LISP_LIST);
  assert (__get_tag (car (cy)) == LISP_ATOM);
  assert (__get_pointer (car (cy)));
  assert (!strcmp (__get_pointer (car (cy)), "1"));
  assert (!strcmp (__get_pointer (car (cdr (cy))), "2"));
  assert (!strcmp (__get_pointer (car (cdr (cdr (cy)))), "3"));
  LOG_END_TESTCASE ();
}

void
test_print ()
{
  LOG_START_TESTCASE ();
  void *a
    = cons (create_atom ("a"),
            cons (create_atom ("b"), cons (create_atom ("c"), NIL)));
  void *b
    = cons (create_atom ("1"),
            cons (create_atom ("2"), cons (create_atom ("3"), NIL)));
  void *ab = cons (a, b);

  printf ("PRINT NIL:\n");
  assert (!strcmp ("()", PRINT (NIL)));
  printf ("PRINT a:\n");
  assert (!strcmp ("(a b c)", PRINT (a)));
  printf ("PRINT b:\n");
  assert (!strcmp ("(1 2 3)", PRINT (b)));
  printf ("PRINT ab:\n");
  assert (!strcmp ("((a b c) 1 2 3)", PRINT (ab)));
  LOG_END_TESTCASE ();
}

void
test_append ()
{
  LOG_START_TESTCASE ();
  void *a
    = cons (create_atom ("a"),
            cons (create_atom ("b"), cons (create_atom ("c"), NIL)));

  void *b
    = cons (create_atom ("1"),
            cons (create_atom ("2"), cons (create_atom ("3"), NIL)));

  void *ab = append (a, b);

  assert (!strcmp (__get_pointer (car (ab)), "a"));
  assert (!strcmp (__get_pointer (car (cdr (ab))), "b"));
  assert (!strcmp (__get_pointer (car (cdr (cdr (ab)))), "c"));
  assert (!strcmp (__get_pointer (car (cdr (cdr (cdr (ab))))), "1"));
  assert (
    !strcmp (__get_pointer (car (cdr (cdr (cdr (cdr (ab)))))), "2"));
  assert (
    !strcmp (__get_pointer (car (cdr (cdr (cdr (cdr (cdr (ab))))))),
             "3"));

  assert (!strcmp ("(a b c 1 2 3)", PRINT (ab)));
  LOG_END_TESTCASE ();
}

void
test_read_list ()
{
  LOG_START_TESTCASE ();
  const char input[] = "(a b c d)";
  void *list = READ (input);

  printf ("%s\n", PRINT (list));
  assert (!strcmp (input, PRINT (list)));
  LOG_END_TESTCASE ();
}

void
test_read_complex_list ()
{
  LOG_START_TESTCASE ();
  const char input[] = "(a b c d (1 2 3 4 (j n k p)) a s d f)";
  void *list = READ (input);

  printf ("%s\n", PRINT (list));
  assert (!strcmp (input, PRINT (list)));
  LOG_END_TESTCASE ();
}

void
test_invalid_input ()
{
  LOG_START_TESTCASE ();
  const char input[] = "(a b c d (1 2 3 4 (j n k p)) a s d f";
  void *list = READ (input);

  printf ("%s\n", PRINT (list));
  //  assert (!strcmp (PRINT(NIL), PRINT(list)));
  LOG_END_TESTCASE ();
}

void
test_concat ()
{
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("test (concat '(1 2 3) NIL)");
  char a[] = "(1 2 3)";
  void *x = READ (a);
  void *xy = concat (x, NIL);
  assert (!strcmp (PRINT (xy), a));
  LOG_END_TEST_STEP ("test (concat '(1 2 3) NIL)");

  LOG_START_TEST_STEP ("test (concat '(1 2 3) '(a b c))");
  char b[] = "(a b c)";
  void *y = READ (b);
  void *ab = concat (x, y);
  printf ("%s\n", PRINT (ab));
  assert (!strcmp (PRINT (ab), "(1 2 3 a b c)"));
  LOG_END_TESTCASE ();

  LOG_START_TEST_STEP ("test (concat '(1 2 3 (a b c)) '(d e f))");
  char lst1[] = "(1 2 3 (a b c))";
  char lst2[] = "(d e f)";
  char expect[] = "(1 2 3 (a b c) d e f)";
  x = READ (lst1);
  y = READ (lst2);
  xy = concat (x, y);
  printf ("%s\n", PRINT (xy));
  assert (!strcmp (PRINT (xy), expect));
  LOG_END_TEST_STEP ("test (concat '(1 2 3 (a b c)) '(d e f))");
}

void
test_copy ()
{
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("test (copy '(1 2 3))");
  char lst[] = "(1 2 3)";
  void *x = READ (lst);
  void *x2 = copy (x);

  printf ("%s\n", PRINT (x2));
  assert (!strcmp (PRINT (x2), lst));
  LOG_END_TESTCASE ();
}

void
test_pair ()
{
  LOG_START_TESTCASE ();
  const char a[] = "(a b c)";
  const char b[] = "(1 2 3)";

  void *x = READ (a);
  void *y = READ (b);

  assert (!strcmp (a, PRINT (x)));
  assert (!strcmp (b, PRINT (y)));

  void *xy = pair (x, y);

  printf ("%s\n", PRINT (xy));
  assert (!strcmp ("((a 1) (b 2) (c 3))", PRINT (xy)));

  LOG_END_TESTCASE ();
}

void
test_eval ()
{
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("eval (car (1 2 3 4))");
  const char a[] = "(car (1 2 3 4))";

  void *l = READ (a);
  l = EVAL (l, NIL);
  printf ("%s\n", PRINT (l));
  assert (!strcmp ("1", PRINT (l)));
  LOG_END_TEST_STEP ("(eval (car (1 2 3 4)))");

  LOG_START_TEST_STEP ("(eval (cdr (1 2 3 4)))");
  const char b[] = "(cdr (1 2 3 4))";
  printf ("%s\n", PRINT (EVAL (READ (b), NIL)));
  assert (!strcmp ("(2 3 4)", PRINT (EVAL (READ (b), NIL))));
  LOG_END_TEST_STEP ("(eval (cdr (1 2 3 4)))");
  LOG_END_TESTCASE ();

  LOG_START_TEST_STEP ("(eval (cons 1 (a b c d))");
  const char c[] = "(cons 1 (a b c d))";
  printf ("%s\n", PRINT (EVAL (READ (c), NIL)));
  assert (!strcmp ("(1 a b c d)", PRINT (EVAL (READ (c), NIL))));
  LOG_END_TEST_STEP ("(eval (cons 1 (a b c d))");

  LOG_START_TEST_STEP ("(eval (cons 1 (cdr (a b c d))))");
  const char d[] = "(cons 1 (cdr (a b c d)))";
  printf ("%s\n", PRINT (EVAL (READ (d), NIL)));
  assert (!strcmp ("(1 b c d)", PRINT (EVAL (READ (d), NIL))));
  LOG_END_TEST_STEP ("(eval (cons 1 (cdr (a b c d))))");

  LOG_START_TEST_STEP ("(eval (quote 1 2 3 4 5))");
  const char e[] = "(quote 1 2 3 4 5)";
  printf ("%s\n", PRINT (EVAL (READ (e), NIL)));
  assert (!strcmp ("(1 2 3 4 5)", PRINT (EVAL (READ (e), NIL))));
  LOG_END_TEST_STEP ("(eval (quote 1 2 3 4 5))");

  LOG_START_TEST_STEP ("(eval (cons (quote a b c) (quote 1 2 3)))");
  const char f[] = "(cons (quote a b c) (quote 1 2 3))";
  printf ("%s\n", PRINT (EVAL (READ (f), NIL)));
  assert (!strcmp ("((a b c) 1 2 3)", PRINT (EVAL (READ (f), NIL))));
  LOG_END_TEST_STEP ("(eval (cons (quote a b c) (quote 1 2 3)))");
}
int
main ()
{
  fprintf (stdout, "Starting tests....\n");
  test_pointer_tagging ();
  test_non_tagged_pointer ();
  test_tagged_malloc ();
  test_tagged_strdup ();
  test_create_atom ();
  test_create_list_node ();
  test_atom ();
  test_cons ();
  test_car ();
  test_cdr ();
  test_cons_with_lists ();
  test_print ();
  test_append ();
  test_read_list ();
  test_read_complex_list ();
  //  test_invalid_input ();
  //  test_concat();
  //  test_copy();
  //  test_pair ();
  test_eval ();
  fprintf (stdout, "All tests executed successfully!\n");
}
