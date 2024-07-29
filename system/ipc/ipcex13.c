#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd;
  off_t offset;
  char *data;
  struct stat sbuf;

  if (argc != 2) {
    fprintf(stderr, "usage: mmapdemo offset\n");
    exit(1);
  }

  if ((fd = open("mmapdemo.c", O_RDONLY)) == -1) {
    perror("open");
    exit(1);
  }

  if (stat("mmapdemo.c", &sbuf) == -1) {
    perror("stat");
    exit(1);
  }

  offset = atoi(argv[1]);
  if (offset < 0 || offset > sbuf.st_size - 1) {
    fprintf(stderr, "mmapdemo: offset must be in the range 0-%ld\n",
            sbuf.st_size - 1);
    exit(1);
  }

  data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  printf("byte at offset %ld is '%c'\n", offset, data[offset]);

  return 0;
}
