#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_NAME "american_maid"

int main(void) {
  char s[300];
  int num, fd;

  // Use mkfifo as mknod is not portable
  // mkfifo(FIFO_NAME, 0666);
  mknod(FIFO_NAME, S_IFIFO | 0666, 0);

  printf("waiting for readers...\n");
  // Open will block until some other process
  // opens the other end for reading.
  fd = open(FIFO_NAME, O_WRONLY);
  printf("got a reader--type some stuff\n");

  while (fgets(s, 300, stdin), !feof(stdin)) {
    if ((num = write(fd, s, strlen(s))) == -1)
      perror("write");
    else
      printf("speak: wrote %d bytes\n", num);
  }

  return 0;
}
