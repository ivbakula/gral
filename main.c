#include "data.h"
#include "repl.h"

#include <readline/history.h>
#include <readline/readline.h>

int
main (int argc, char *argv[])
{
  while (1)
    {
      char *input = readline ("repl> ");
      add_history (input);

      if (!input)
        break;

      printf ("%s\n", PRINT (EVAL (READ (input), NIL)));
    }
}
