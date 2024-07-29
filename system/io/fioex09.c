/*
 C program to read contents of a file line by line
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1000

int main() {
  FILE* fp;
  char linebuf[MAX_LINE_LENGTH];

  fp = fopen("readme.txt", "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }
  while (fgets(linebuf, MAX_LINE_LENGTH, fp) != NULL) {
    printf("%s", linebuf);
  }
  fclose(fp);
  return 0;
}
