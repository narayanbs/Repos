/* Handling partial sends */
#include <sys/socket.h>
#include <sys/types.h>

int sendall(int fd, char *buf, int *len) {
  int bytessent = 0;     // how many bytes we've sent
  int bytesleft = *len;  // how many we have left to send
  int n;

  while (bytessent < *len) {
    n = send(fd, buf + bytessent, bytesleft, 0);
    if (n == -1) {
      break;
    }
    bytessent += n;
    bytesleft -= n;
  }

  *len = bytessent;  // return number actually sent here

  return n == -1 ? -1 : 0;  // return -1 on failure, 0 on success
}
