#include <readline/history.h>
#include <readline/readline.h>
#include "axioms.h"
#include "data.h"
#include "repl.h"

extern void *env;

int
main (int argc, char *argv[])
{
  env = NULL;
  while (1)
    {
      char *input = readline ("repl> ");
      add_history (input);

      if (!input)
        break;

      printf ("%s\n", PRINT (EVAL (READ (input))));
    }
}
