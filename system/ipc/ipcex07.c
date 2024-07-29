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

  mknod(FIFO_NAME, S_IFIFO | 0666, 0);

  printf("waiting for writers...\n");
  // open will block until some other process
  // opens the pipe for writing.
  fd = open(FIFO_NAME, O_RDONLY);
  printf("got a writer\n");

  do {
    if ((num = read(fd, s, 300)) == -1)
      perror("read");
    else {
      s[num] = '\0';
      printf("tick: read %d bytes: \"%s\"\n", num, s);
    }
  } while (num > 0);

  return 0;
}
