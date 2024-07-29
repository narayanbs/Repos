/*
 * Program to update a global variable using mmap
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

static int *glob_var;

int main(int argc, char *argv[]) {
  glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANON, -1, 0);
  *glob_var = 1;

  if (fork() == 0) {
    *glob_var = 5;
    exit(EXIT_SUCCESS);
  } else {
    wait(NULL);
    printf("%d\n", *glob_var);
    munmap(glob_var, sizeof *glob_var);
  }
  return EXIT_SUCCESS;
}
