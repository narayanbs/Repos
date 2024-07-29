/*
 C program to read all the contents of a file
*/
#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE* textfile;
  char* text;
  long numbytes;

  textfile = fopen("readme.txt", "r");
  if (textfile == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }

  fseek(textfile, 0, SEEK_END);
  numbytes = ftell(textfile);
  fseek(textfile, 0, SEEK_SET);

  text = (char*)calloc(numbytes, sizeof(char));
  if (text == NULL) {
    exit(EXIT_FAILURE);
  }
  fread(text, sizeof(char), numbytes, textfile);
  fclose(textfile);
  printf(text);
  return 0;
}
