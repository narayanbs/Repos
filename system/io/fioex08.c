/*
 C program to read all the contents of a file
*/
#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE* fp;
  char* buf;
  long numbytes;

  // By default fopen uses 0666 as the mode permission.
  fp = fopen("readme.txt", "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fseek(fp, 0, SEEK_END);
  numbytes = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  buf = (char*)calloc(numbytes, sizeof(char));
  if (buf == NULL) {
    exit(EXIT_FAILURE);
  }
  fread(buf, sizeof(char), numbytes, fp);
  fclose(fp);
  printf("%s", buf);
  free(buf);
  return 0;
}
