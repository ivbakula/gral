#ifndef __TRACE_H
#define __TRACE_H

#include <stdio.h>

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* Tracing macros */
#define TRACE_SYSTEM_ERROR(fmt, ...)                        \
  fprintf (stderr, "ERROR in %s:%d! Error msg = " fmt "\n", \
           __FILE__, __LINE__, ##__VA_ARGS__)

#define TRACE_LANGUAGE_ERROR(fmt, ...)                     \
  fprintf (stderr, "Error in %s: " fmt "\n", __FUNCTION__, \
           ##__VA_ARGS__)

#define LOG_START_TESTCASE()                          \
  fprintf (stdout,                                    \
           ANSI_COLOR_CYAN                            \
           "Starting %s ...\n...\n" ANSI_COLOR_RESET, \
           __FUNCTION__)

#define LOG_END_TESTCASE()                                     \
  fprintf (stdout,                                             \
           ANSI_COLOR_GREEN                                    \
           "Test %s Executed successfully\n" ANSI_COLOR_RESET, \
           __FUNCTION__)

#define LOG_START_TEST_STEP(fmt, ...)                 \
  fprintf (stdout,                                    \
           ANSI_COLOR_BLUE "Executing Test step " fmt \
                           "\n" ANSI_COLOR_RESET,     \
           ##__VA_ARGS__)

#define LOG_END_TEST_STEP(fmt, ...)                     \
  fprintf (stdout,                                      \
           ANSI_COLOR_GREEN                             \
           "Test step " fmt                             \
           " executed successfuly \n" ANSI_COLOR_RESET, \
           ##__VA_ARGS__)
#endif
