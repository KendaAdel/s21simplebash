#define _GNU_SOURCE
#include "s21_cat.h"

int main(int argc, char *argv[]) {
  flags flag = {0};
  if (parser(argc, argv, &flag)) {
    read(argc, argv, &flag);
  }
  return 0;
}

bool parser(int argc, char *argv[], flags *flag) {
  int rez;
  bool result = true;
  bool exit_proces = false;
  const char *short_options = "benstvTE";

  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"show-ends", no_argument, NULL, 'E'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {"show-tabs", no_argument, NULL, 'T'},
      {"show-nonprinting", no_argument, NULL, 'v'},
      {0, 0, 0, 0}};
  while (!exit_proces && (rez = getopt_long(argc, argv, short_options,
                                            long_options, NULL)) != -1) {
    switch (rez) {
      case 'b':
        flag->b = true;
        break;
      case 'e':
        flag->e = flag->v = true;
        break;
      case 'n':
        flag->n = true;
        break;
      case 's':
        flag->s = true;
        break;
      case 't':
        flag->t = flag->v = true;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'T':
        flag->T = 1;
        break;
      case 'E':
        flag->E = 1;
        break;
      default:
        exit_proces = true;
        result = false;
    }
  }
  if (flag->b == true) flag->n = false;
  if (flag->e == true) flag->v = true;
  if (flag->E == true) flag->v = false;
  if (flag->T == true) flag->v = flag->t = true;

  if (optind >= argc) {
    fprintf(stderr, "Error: file not specified for reading\n");
    return false;
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

void read(int argc, char *argv[], flags *flag) {
  for (flag->countFile = optind; flag->countFile < argc; flag->countFile++) {
    FILE *file = fopen(argv[flag->countFile], "r");
    if (file == NULL) {
      perror("Error opening file");
      continue;
    } else {
      tput(flag, file);
    }
  }
}
void tput(flags *flag, FILE *file) {
  int letter = '\0';
  bool myflag = false;
  int count = (flag->countFile - optind) ? 0 : 1;
  while ((letter = fgetc(file)) != EOF) {
    if (ferror(file)) {
      perror("Error reading file");
      break;
    }
    if (flag->s) {
      if (letter == '\n') {
        if (count >= 2) continue;
        count++;
      } else {
        count = 0;
      }
    }
    if (flag->n && (!flag->lineNumber || myflag) || letter == '\0') {
      printf("%6d\t", ++flag->lineNumber);
      myflag = false;
    }
    if (flag->b && (myflag || !flag->lineNumber) && letter != '\n') {
      printf("%6d\t", ++flag->lineNumber);
      myflag = false;
    }
    if ((flag->E || flag->e) && letter == '\n') {
      putchar('$');
    }
    if (flag->t && letter == '\t') {
      printf("^I");
      continue;
    }
    if (letter == '\n') {
      myflag = true;
    }
    if (flag->v) {
      v_print(letter);
    } else {
      putchar(letter);
    }
  }
  fclose(file);
}
