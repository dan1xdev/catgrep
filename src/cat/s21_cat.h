#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int v;
  int E;
  int T;
  int t;
} Flags;

void printCharWithFlags(int c, Flags flags);
void catPrint(Flags flags, FILE* file);
void processFiles(Flags flags, int fileIndex, int argc, char** argv);
void printUsage(char* programName);

#endif