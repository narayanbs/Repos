/* Handling partial sends */
#include <sys/socket.h>
#include <sys/types.h>

// Send the entire buffer over a socket, even if send()
// only transmits part of it each call.
//
// fd   = socket file descriptor
// buf  = data buffer to send
// len  = input: total bytes to send
//        output: total bytes actually sent
//
// Returns:
//   0  on success (everything sent)
//  -1  on failure
int sendall(int fd, char *buf, int *len) {
  // Total number of bytes successfully sent so far
  int bytessent = 0;

  // Number of bytes remaining to send
  int bytesleft = *len;

  // Number of bytes returned by a single send() call
  int n;

  // Keep sending until we've sent the original length
  while (bytessent < *len) {
    // Attempt to send the remaining bytes.
    //
    // fd              -> socket
    // buf + bytessent -> pointer to unsent data
    // bytesleft       -> number of bytes still pending
    // 0               -> no special flags
    n = send(fd, buf + bytessent, bytesleft, 0);

    // send() failed
    if (n == -1) {
      // Exit loop immediately on error
      break;
    }

    // Add newly sent bytes to total count
    bytessent += n;

    // Reduce remaining byte count
    bytesleft -= n;
  }

  // Store actual bytes sent back into caller's len
  *len = bytessent;

  // If send() ever failed, return -1.
  // Otherwise return success.
  return n == -1 ? -1 : 0;
}
