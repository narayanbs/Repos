/*
 C program to read file character by character
*/
#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE* fp;
  char ch;

  fp = fopen("readme.txt", "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }
  while ((ch = fgetc(fp)) != EOF) {
    putchar(ch);
  }
  fclose(fp);
  return 0;
}
