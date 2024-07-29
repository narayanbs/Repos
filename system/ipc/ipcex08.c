#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct flock fl = {
      .l_type = F_WRLCK,    /* F_RDLCK, F_WRLCK, F_UNLCK      */
      .l_whence = SEEK_SET, /* SEEK_SET, SEEK_CUR, SEEK_END   */
      .l_start = 0,         /* Offset from l_whence           */
      .l_len = 0,           /* length, 0 = to EOF             */
      // .l_pid             /* PID holding lock; F_RDLCK only */
  };
  int fd;

  if (argc > 1)
    fl.l_type = F_RDLCK;

  if ((fd = open("lockdemo.c", O_RDWR)) == -1) {
    perror("open");
    exit(1);
  }

  printf("Press <RETURN> to try to get lock: ");
  getchar();
  printf("Trying to get lock...");

  /* F_GETLK, F_SETLK, F_SETLKW */
  if (fcntl(fd, F_SETLKW, &fl) == -1) {
    perror("fcntl");
    exit(1);
  }

  printf("got lock\n");
  printf("Press <RETURN> to release lock: ");
  getchar();

  fl.l_type = F_UNLCK; /* set to unlock same region */

  if (fcntl(fd, F_SETLK, &fl) == -1) {
    perror("fcntl");
    exit(1);
  }

  printf("Unlocked.\n");

  close(fd);

  return 0;
}
