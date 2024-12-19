#include "s21_cat.h"

int main(int argc, char *argv[]) {
  flags flags = {0};
  if (parser(argc, argv, &flags)) {
    readFile(argc, argv, &flags);
  }
}

bool parser(int argc, char **argv, flags *flags) {
  int opt;
  bool result = true;
  bool exit_proces = false;
  struct option long_option[] = {{"number-nonblank", no_argument, NULL, 'b'},
                                 {"number", no_argument, NULL, 'n'},
                                 {"squeeze-blank", no_argument, NULL, 's'},
                                 {0, 0, 0, 0}};
  while (!exit_proces &&
         (opt = getopt_long(argc, argv, "benstETv", long_option, NULL)) != -1) {
    switch (opt) {
      case 'b':
        flags->b = true;
        break;
      case 'e':
        flags->e = true;
        flags->v = true;
        break;
      case 'n':
        flags->n = true;
        break;
      case 's':
        flags->s = true;
        break;
      case 't':
        flags->t = true;
        flags->v = true;
        break;
      case 'E':
        flags->e = true;
        break;
      case 'T':
        flags->t = true;
        break;
      case 'v':
        flags->v = true;
        break;
      default:
        fprintf(stderr, "cat: invalid option -- '%c'\n", opt);
        exit_proces = true;
        result = false;
    }
    if (result == false) {
      exit_proces = true;
    }
    if (flags->b) flags->n = false;
  }
  return result;
}

void v_print(unsigned char c) {
  if (c == 9 || c == 10) {
    printf("%c", c);
  } else if (c >= 32 && c < 127) {
    printf("%c", c);
  } else if (c == 127) {
    printf("^?");
  } else if (c >= 128 + 32) {
    printf("M-");
    (c < 128 + 127) ? printf("%c", c - 128) : printf("^?");
  } else {
    (c > 32) ? printf("M-^%c", c - 128 + 64) : printf("^%c", c + 64);
  }
}

void readFile(int argc, char **argv, flags *flags) {
  for (flags->countFile = optind; flags->countFile < argc; flags->countFile++) {
    FILE *fp = fopen(argv[flags->countFile], "r");
    if (fp == NULL)
      perror("Error opening file");
    else
      work(flags, fp);
  }
}

void work(flags *flags, FILE *fp) {
  int c;
  bool myflag = false;
  int count = (flags->countFile - optind) ? 0 : 1;
  while ((c = fgetc(fp)) != EOF) {
    if (flags->s) {
      if (c == '\n') {
        if (count >= 2) continue;
        count++;
      } else {
        count = 0;
      }
    }
    if ((flags->n && (!flags->lineNumber || myflag)) ||
        (flags->b && (myflag || !flags->lineNumber) && c != '\n')) {
      printf("%6d\t", ++flags->lineNumber);
      myflag = false;
    }
    if (flags->e && c == '\n') {
      putchar('$');
    }
    if (flags->t && c == '\t') {
      printf("^I");
      continue;
    }
    if (c == '\n') {
      myflag = true;
    }
    if (flags->v) {
      v_print(c);
    } else {
      putchar(c);
    }
  }
  fclose(fp);
}