#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  int fdOne, fdTwo, fdThree;

  fdOne = open("misc.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);

  if (fdOne < 0) {
    printf("Error opening / creating test.txt. ");
    if (errno == EACCES) {
      printf("Error #%d (EACCES): Permission denied.\n", EACCES);
      exit(EXIT_FAILURE);
    }
  } else {
    printf("misc.txt created / opened ");
    printf("with file descriptor %d.\n", fdOne);
  }

  // lets use the fcntl() function to copy the
  // file descriptor
  if ((fdTwo = fcntl(fdOne, F_DUPFD, 0)) < 0) {
    printf("Error duplicating file descriptor.\n");
    exit(EXIT_FAILURE);
  } else {
    printf("File descriptor duplicated. ");
    printf("New file descriptor is %d.\n", fdTwo);
  }

  // set the file descriptor to be a higher number;

  if ((fdThree = fcntl(fdOne, F_DUPFD, 11)) < 0) {
    printf("Error duplicating file descriptor.\n");
    exit(EXIT_FAILURE);
  } else {
    printf("File descriptor duplicated. ");
    printf("New file descriptor is %d.\n", fdThree);
  }

  close(fdOne);
  close(fdTwo);
  close(fdThree);

  return 0;
}
