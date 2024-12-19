#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool b;
  bool e;
  bool n;
  bool s;
  bool t;
  bool v;
  int lineNumber;
  int countFile;
} flags;

bool parser(int argc, char **argv, flags *flags);
void v_print(unsigned char c);
void work(flags *flags, FILE *fp);
void readFile(int argc, char **argv, flags *flags);
