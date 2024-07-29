/*
 C program to read and write data types to a file
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Course {
  char cname[30];
  char sdate[30];
};

int main() {
  FILE* of;
  of = fopen("test.txt", "w");
  if (of == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }
  struct Course c1 = {.cname = "Algorithms", .sdate = "3rd Oct"};
  struct Course c2 = {.cname = "Datastructures", .sdate = "4th Nov"};
  struct Course c3 = {.cname = "Programming", .sdate = "5th Dec"};

  /* Note -- the following is illegal in C.
  Arrays can be assigned values during initialization, but they cannot
  be assigned after initialization. the following will throw up a Error

  struct Course c4;
  c4.cname = "Machine Learning" ;

  we need to use strcpy(c4.cname, "Machine Learning");
  */

  unsigned long status = 0;

  status = fwrite(&c1, sizeof(struct Course), 1, of);
  if (status == 0) {
    fprintf(stderr, "Error writing content\n");
    exit(EXIT_FAILURE);
  }
  status = fwrite(&c2, sizeof(struct Course), 1, of);
  if (status == 0) {
    fprintf(stderr, "Error writing content\n");
    exit(EXIT_FAILURE);
  }

  status = fwrite(&c3, sizeof(struct Course), 1, of);
  if (status == 0) {
    fprintf(stderr, "Error writing content\n");
    exit(EXIT_FAILURE);
  }

  fclose(of);

  FILE* inf;
  inf = fopen("test.txt", "r");
  if (inf == NULL) {
    fprintf(stderr, "Error opening file\n");
    exit(EXIT_FAILURE);
  }
  struct Course inp;
  memset(&inp, 0, sizeof(struct Course));
  while (fread(&inp, sizeof(struct Course), 1, inf) != 0) {
    printf("Course Name = %s, Started = %s\n", inp.cname, inp.sdate);
  }
  fclose(inf);
}
