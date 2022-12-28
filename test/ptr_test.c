#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../axioms.h"
#include "../data.h"
#include "../pointer.h"
#include "../repl.h"
#include "../trace.h"

extern void *env;

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
test_pair ()
{
  LOG_START_TESTCASE ();

  LOG_START_TEST_STEP ("(pair (a b c) (1 2 3))");
  char a[] = "(a b c)";
  char b[] = "(1 2 3)";

  void *x = READ (a);
  void *y = READ (b);

  void *xy = pair (x, y);
  printf ("%s\n", PRINT (xy));
  assert (!strcmp (PRINT (xy), "((a 1) (b 2) (c 3))"));
  LOG_END_TESTCASE ();
}

void
test_assoc ()
{
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("(assoc a ((a 1) (b 2) (c 3)))");
  char a[] = "(a b c)";
  char b[] = "(1 2 3)";
  void *x = READ (a);
  void *y = READ (b);
  void *xy = pair (x, y);
  printf ("(assoc a %s)\n", PRINT (xy));
  void *r = assoc (create_atom ("a"), xy);
  assert (!strcmp ("1", PRINT (r)));
  LOG_END_TESTCASE ();

  printf ("(assoc b %s)\n", PRINT (xy));
  assert (!strcmp ("2", PRINT (assoc (create_atom ("b"), xy))));
  printf ("(assoc c %s)\n", PRINT (xy));
  assert (!strcmp ("3", PRINT (assoc (create_atom ("c"), xy))));
  printf ("(assoc z %s)\n", PRINT (xy));
  assert (NIL == assoc (create_atom ("z"), xy));
}

void
test_eval ()
{
  env = NIL;
  LOG_START_TESTCASE ();
  LOG_START_TEST_STEP ("eval (car (quote (1 2 3 4)))");
  const char a[] = "(car (quote (1 2 3 4)))";

  void *l = READ (a);
  l = EVAL (l);
  printf ("%s\n", PRINT (l));
  assert (!strcmp ("1", PRINT (l)));
  LOG_END_TEST_STEP ("(eval (car (1 2 3 4)))");

  LOG_START_TEST_STEP ("(eval (cdr (quote (1 2 3 4))))");
  const char b[] = "(cdr (quote (1 2 3 4)))";
  printf ("%s\n", PRINT (EVAL (READ (b))));
  assert (!strcmp ("(2 3 4)", PRINT (EVAL (READ (b)))));
  LOG_END_TEST_STEP ("(eval (cdr (quote (1 2 3 4))))");
  LOG_END_TESTCASE ();

  LOG_START_TEST_STEP ("(eval (cons 1 (a b c d))");
  const char c[] = "(cons (quote 1) (quote (a b c d)))";
  printf ("%s\n", PRINT (EVAL (READ (c))));
  assert (!strcmp ("(1 a b c d)", PRINT (EVAL (READ (c)))));
  LOG_END_TEST_STEP ("(eval (cons 1 (quote (a b c d)))");

  LOG_START_TEST_STEP ("(eval (cons 1 (cdr (a b c d))))");
  const char d[] = "(cons (quote 1) (cdr (quote (a b c d))))";
  printf ("%s\n", PRINT (EVAL (READ (d))));
  assert (!strcmp ("(1 b c d)", PRINT (EVAL (READ (d)))));
  LOG_END_TEST_STEP ("(eval (cons 1 (cdr (quote (a b c d)))))");

  LOG_START_TEST_STEP ("(eval (quote 1 2 3 4 5))");
  const char e[] = "(quote (1 2 3 4 5))";
  printf ("%s\n", PRINT (EVAL (READ (e))));
  assert (!strcmp ("(1 2 3 4 5)", PRINT (EVAL (READ (e)))));
  LOG_END_TEST_STEP ("(eval (quote 1 2 3 4 5))");

  LOG_START_TEST_STEP ("(eval (cons (quote a b c) (quote 1 2 3)))");
  const char f[] = "(cons (quote (a b c)) (quote (1 2 3)))";
  printf ("%s\n", PRINT (EVAL (READ (f))));
  assert (!strcmp ("((a b c) 1 2 3)", PRINT (EVAL (READ (f)))));
  LOG_END_TEST_STEP ("(eval (cons (quote a b c) (quote 1 2 3)))");

  LOG_START_TEST_STEP ("(eval (pair (1 2 3) (a b c)))");
  const char g[] = "(pair (quote (1 2 3)) (quote (a b c)))";
  printf ("%s\n", PRINT (EVAL (READ (g))));
  assert (!strcmp ("((1 a) (2 b) (3 c))", PRINT (EVAL (READ (g)))));
  LOG_END_TEST_STEP ("(eval (pair (1 2 3) (a b c)))");

  LOG_START_TEST_STEP ("(eval (assoc 1 %s))",
                       PRINT (EVAL (READ (g))));
  const char h[] = "(assoc (quote 1) ((1 a) (2 b) (3 c)))";
  printf ("%s\n", PRINT (EVAL (READ (h))));
  assert (!strcmp ("a", PRINT (EVAL (READ (h)))));
  LOG_END_TEST_STEP ("(eval (assoc 1 %s))", PRINT (EVAL (READ (h))));

  LOG_START_TEST_STEP ("(eval (set (a) (1)))");
  const char i[] = "(setq a (quote 1))";
  EVAL (READ (i));
  printf ("%s\n", PRINT (env));
  assert (!strcmp (PRINT (env), "((a 1))"));
  LOG_END_TEST_STEP ("(eval (set (a) (1)))");

  const char j[] = "(setq b (quote 2))";
  LOG_START_TEST_STEP ("(eval %s\n)", j);
  EVAL (READ (j));
  printf ("%s\n", PRINT (env));
  assert (!strcmp (PRINT (env), "((b 2) (a 1))"));
  LOG_START_TEST_STEP ("(eval (set b 2))");

  const char k[] = "(setq c (quote (1 2 3 4)))";
  LOG_START_TEST_STEP ("(eval %s\n)", k);
  EVAL (READ (k));
  printf ("%s\n", PRINT (env));
  //  assert (!strcmp(PRINT(env), "(())"))
  LOG_END_TEST_STEP ("(eval %s\n)", k);
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
  test_pair ();
  test_assoc ();
  test_eval ();
  fprintf (stdout, "All tests executed successfully!\n");
}
