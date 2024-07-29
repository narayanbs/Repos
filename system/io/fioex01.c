/*
 * access mode = O_RDONLY, O_WRONLY, O_RDWR
 * creation flags
 *   O_CREAT
 *   O_CREAT | O_TRUNC   -- only for O_WRONLY, O_RDWR
 *   O_CREAT | O_EXCL
 *   O_CREAT | O_APPEND
 *   O_APPEND
 *   O_APPEND | O_TRUNC   -- only for O_WRONLY, O_RDWR
 */   

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argx, char *argv[]) {
  int pfd;
  char filename[1024];
  strcpy(filename, argv[1]);

  // Opening a file for writing
  // mode_t is required if O_CREAT or O_TMPFILE are used
  // here we use 0666 (default for ubuntu)
  int fg = O_WRONLY | O_CREAT | O_TRUNC;
  int perms = 0666;
  if ((pfd = open(filename, fg, 0666)) == -1) {
    perror("Cannot open file:\n");
    exit(1);
  }

  //or 

  mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  // Opening a file for writing
  // perms are required if O_CREAT or O_TMPFILE are used
  int fg = O_WRONLY | O_CREAT | O_TRUNC;
  if ((pfd = open(filename, fg, perms)) == -1) {
    perror("Cannot open file:\n");
    exit(1);
  }

  // Opening a file for appending, file needs to exist
  // perms are not required for an existing file
  fg = O_WRONLY | O_APPEND;
  if ((pfd = open(filename, fg)) == -1) {
    perror("Cannot open file:\n");
    exit(1);
  }

  // Opening a file for appending, creating if it doesn't exist
  fg = O_WRONLY | O_CREAT | O_APPEND;
  if ((pfd = open(filename, fg, perms)) == -1) {
    perror("Cannot open file:\n");
    exit(1);
  }

  // openind a file using an existence check
  fg = O_WRONLY | O_CREAT | O_EXCL;
  if ((pfd = open(filename, fg, perms)) == -1) {
    perror("Cannot open file:\n");
    exit(1);
  }
  return 0;
}
