#include "s21_cat.h"

int main(int argc, char** argv) {
  Flags flags = {0};
  int opt;
  int return_code = 0;

  while ((opt = getopt(argc, argv, "bensvETt")) != -1) {
    switch (opt) {
      case 'b':
        flags.b = 1;
        break;
      case 'e':
        flags.e = 1;
        break;
      case 'n':
        flags.n = 1;
        break;
      case 's':
        flags.s = 1;
        break;
      case 'v':
        flags.v = 1;
        break;
      case 'E':
        flags.e = 1;
        break;
      case 'T':
        flags.t = 1;
        break;
      case 't':
        flags.t = 1;
        break;
      case '?':
        printUsage(argv[0]);
        return_code = 1;
        break;
    }
  }

  if (return_code == 0) {
    processFiles(flags, optind, argc, argv);
  }

  return return_code;
}

void printCharWithFlags(int c, Flags flags) {
  if (c == '\n') {
    if (flags.e) {
      printf("$");
    }
    putchar(c);
  } else if (c == '\t') {
    if (flags.t) {
      printf("^I");
    } else {
      putchar(c);
    }
  } else {
    int show_nonprinting = flags.v;
    if (flags.e) show_nonprinting = 1;
    if (flags.t) show_nonprinting = 1;

    if (show_nonprinting) {
      if (c >= 0 && c <= 31 && c != '\t' && c != '\n') {
        printf("^%c", c + 64);
      } else if (c == 127) {
        printf("^?");
      } else if (c >= 128 && c <= 159) {
        printf("M-^%c", c - 64);
      } else if (c >= 160 && c <= 255) {
        printf("M-%c", c - 128);
      } else {
        putchar(c);
      }
    } else {
      putchar(c);
    }
  }
}

void processFiles(Flags flags, int fileIndex, int argc, char** argv) {
  if (fileIndex >= argc) {
    catPrint(flags, stdin);
    return;
  }

  for (int i = fileIndex; i < argc; i++) {
    FILE* file = fopen(argv[i], "r");
    if (file != NULL) {
      catPrint(flags, file);
      fclose(file);
    } else {
      fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
    }
  }
}

void catPrint(Flags flags, FILE* file) {
  int c;
  int line_number = 1;
  int empty_line_count = 0;
  int prev_char = '\n';

  while ((c = fgetc(file)) != EOF) {
    int skip_char = 0;

    if (flags.s && c == '\n' && prev_char == '\n') {
      empty_line_count++;
      if (empty_line_count > 1) {
        skip_char = 1;
      }
    } else {
      empty_line_count = 0;
    }

    if (skip_char) {
      prev_char = c;
      continue;
    }

    if (prev_char == '\n') {
      if (flags.b && c != '\n') {
        printf("%6d\t", line_number++);
      } else if (flags.n && !flags.b) {
        printf("%6d\t", line_number++);
      }
    }

    if (flags.b && flags.e && prev_char == '\n' && c == '\n') {
      printf("      \t");
    }
    printCharWithFlags(c, flags);

    prev_char = c;
  }
}

void printUsage(char* programName) {
  fprintf(stderr, "Usage: %s [-bensvET] [file ...]\n", programName);
}