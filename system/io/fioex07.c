#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  int fd;
  long numbytes;
  fd = open("readme.txt", O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  numbytes = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  char* buf = (char*)calloc(numbytes, sizeof(char));
  int st = read(fd, buf, numbytes);
  if (st == -1) {
    perror("Error reading file");
    exit(EXIT_FAILURE);
  }
  printf("%s", buf);
  return 0;
  close(fd);
}
