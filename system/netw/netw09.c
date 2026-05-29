/* Handling partial sends */

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

/*
 * sendall()
 *
 * Repeatedly calls send() until either:
 *   1. All requested bytes are transmitted
 *   2. An unrecoverable error occurs
 *
 * Parameters:
 *   fd   -> connected socket file descriptor
 *   buf  -> data buffer to transmit
 *   len  -> input:  total bytes requested to send
 *           output: actual bytes successfully sent
 *
 * Returns:
 *   0  on success (all bytes sent)
 *  -1  on failure (partial or no transmission)
 */
int sendall(int fd, const char *buf, size_t *len) {
  // Total bytes successfully sent so far
  size_t total = 0;

  // Number of bytes still remaining to send
  size_t bytesleft = *len;

  // Number of bytes returned from a single send() call
  //
  // ssize_t is the correct POSIX return type for send():
  //   > 0  -> bytes sent
  //   = 0  -> connection closed / nothing sent
  //   = -1 -> error
  ssize_t n;

  // Continue sending until we've transmitted everything
  while (total < *len) {
    /*
     * Attempt to send remaining bytes.
     *
     * fd            -> socket descriptor
     * buf + total   -> pointer to unsent portion of buffer
     * bytesleft     -> remaining bytes to transmit
     * MSG_NOSIGNAL  -> prevents SIGPIPE if peer disconnected
     */
    n = send(fd, buf + total, bytesleft, MSG_NOSIGNAL);

    // send() failed
    if (n == -1) {
      /*
       * EINTR means the system call was interrupted
       * by a signal before any data was sent.
       *
       * This is recoverable, so retry immediately.
       */
      if (errno == EINTR) continue;

      // Any other error is fatal
      break;
    }

    /*
     * send() returning 0 usually means:
     *   - connection closed
     *   - unable to send further data
     *
     * Prevents infinite loop.
     */
    if (n == 0) break;

    // Update total bytes successfully sent
    total += n;

    // Reduce remaining byte count
    bytesleft -= n;
  }

  // Report actual bytes sent back to caller
  *len = total;

  /*
   * Success only if every requested byte
   * was transmitted.
   */
  return (bytesleft == 0) ? 0 : -1;
}
