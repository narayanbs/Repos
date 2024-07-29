/*
 * Writing file
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("file path must be mentioned\n");
    exit(0);
  }

  const char *filepath = argv[1];
  int fd = open(filepath, O_RDWR);
  if (fd < 0) {
    printf("Could not open file %s", filepath);
    exit(1);
  }

  struct stat statbuf;
  int err = fstat(fd, &statbuf);
  if (err < 0) {
    printf("Could not stat file %s\n", filepath);
    exit(2);
  }

  char *ptr =
      mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mapping failed");
    return 1;
  }
  close(fd);

  for (size_t i = 0; i < statbuf.st_size / 2; i++) {
    int j = statbuf.st_size - i - 1;
    int t = ptr[i];
    ptr[i] = ptr[j];
    ptr[j] = t;
  }

  ssize_t n = write(1, ptr, statbuf.st_size);
  if (n != statbuf.st_size) {
    printf("Write failed\n");
  }

  err = munmap(ptr, statbuf.st_size);
  if (err != 0) {
    perror("Unmapping failed");
    return 1;
  }
  return 0;
}
