/*
 * Program that prints properties of a file
 * stat, lstat and fstat can be used,
 * lstat is similar to stat
 * fstat takes a file descriptor
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
  struct stat sb;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd < 1) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    exit(EXIT_FAILURE);
  }

  printf("ID of containing device:  [%jx,%jx]\n", (uintmax_t)major(sb.st_dev),
         (uintmax_t)minor(sb.st_dev));

  printf("File type:                ");

  // st_mode gives you a bitmask of filetype and permissions)
  switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
      printf("block device\n");
      break;
    case S_IFCHR:
      printf("character device\n");
      break;
    case S_IFDIR:
      printf("directory\n");
      break;
    case S_IFIFO:
      printf("FIFO/pipe\n");
      break;
    case S_IFLNK:
      printf("symlink\n");
      break;
    case S_IFREG:
      printf("regular file\n");
      break;
    case S_IFSOCK:
      printf("socket\n");
      break;
    default:
      printf("unknown?\n");
      break;
  }

  printf("I-node number:            %ju\n", (uintmax_t)sb.st_ino);

  printf("Mode:                     %jo (octal)\n", (uintmax_t)sb.st_mode);

  printf("Link count:               %ju\n", (uintmax_t)sb.st_nlink);
  printf("Ownership:                UID=%ju   GID=%ju\n", (uintmax_t)sb.st_uid,
         (uintmax_t)sb.st_gid);

  printf("Preferred I/O block size: %jd bytes\n", (intmax_t)sb.st_blksize);
  printf("File size:                %jd bytes\n", (intmax_t)sb.st_size);
  printf("Blocks allocated:         %jd\n", (intmax_t)sb.st_blocks);

  printf("Last status change:       %s", ctime(&sb.st_ctime));
  printf("Last file access:         %s", ctime(&sb.st_atime));
  printf("Last file modification:   %s", ctime(&sb.st_mtime));

  printf("\n Constants in fcntl -------------------------------------\n");

  printf("duplicate an existing file descriptor (F_DUPFD) %d\n", F_DUPFD);
  printf("get file descriptor flags (F_GETFD) %d\n", F_GETFD);
  printf("set file descriptor flags (F_SETFD) %d\n", F_SETFD);
  printf("get file status flags (F_GETFL) %d\n", F_GETFL);
  printf("set file status flags (F_SETFL) %d\n", F_SETFL);
  printf("get record locks (F_GETLK) %d\n", F_GETLK);
  printf("set record locks (F_SETLK) %d\n", F_SETLK);
  printf("get asynchronous I/O ownership (F_GETOWN) %d\n", F_GETOWN);
  printf("set asynchronous I/O ownership (F_SETOWN) %d\n", F_SETOWN);

  return 0;
}
