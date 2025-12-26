#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  int fd = open("test.txt", O_CREAT | O_RDWR, 0666);
  if (fd < 0) {
    perror("open");
    return EXIT_FAILURE;
  }

  int newfd;
  char *s = "Jumping jammy this goes into the file\n";

  // FCNTL -------------------------------

  // close and get stdout
  close(STDOUT_FILENO);
  newfd = fcntl(fd, F_DUPFD, STDOUT_FILENO);
  write(newfd, s, strlen(s));

  // DUP call --------------------------------

  // close and get stdout
  close(STDOUT_FILENO);
  newfd = dup(fd);
  write(newfd, s, strlen(s));

  // DUP2 call --------------------------------

  // the following closes STDOUT_FILENO atomically before duplicating
  newfd = dup2(fd, STDOUT_FILENO);
  write(newfd, s, strlen(s));

  close(newfd);
  close(fd);

  // ------------------- More Fcntl examples

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

  close(fdTwo);
  close(fdThree);
  close(fdOne);

  return 0;
}
