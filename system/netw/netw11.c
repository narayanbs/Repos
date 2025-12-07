/**
 * @file multicast_listener.c
 * @brief A simple multicast UDP listener example in C.
 *
 * This program joins a multicast group, binds to a UDP port, and
 * continuously listens for incoming multicast messages. It prints
 * the received messages to standard output.
 *
 * Multicast Group: 239.255.255.250
 * Port: 12345
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MULTICAST_GROUP "239.255.255.250"
#define PORT 12345
#define BUFFER_SIZE 1024

/**
 * @brief Entry point of the multicast listener program.
 *
 * Creates a UDP socket, binds to the specified port, joins the
 * multicast group, and waits indefinitely for incoming datagrams.
 *
 * @return int Exit status: 0 on success, nonzero on fatal error.
 */
int main() {
  int sock;
  struct sockaddr_in addr, src_addr;
  struct ip_mreq mreq;
  char buffer[BUFFER_SIZE];
  socklen_t addr_len = sizeof(struct sockaddr_in);

  /**
   * Create UDP socket.
   */
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  /**
   * Prepare local address structure for binding.
   * INADDR_ANY allows receiving on all available interfaces.
   */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  /**
   * Bind the socket to the chosen port.
   */
  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  /**
   * Convert multicast group string to binary form.
   */
  if (inet_pton(AF_INET, MULTICAST_GROUP, &(mreq.imr_multiaddr)) < 0) {
    perror("multicast group address creation failed");
    exit(EXIT_FAILURE);
  }

  /**
   * Specify interface for receiving multicast.
   * INADDR_ANY selects the default interface.
   */
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  /**
   * Join the multicast group.
   */
  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  printf("Listening for messages on multicast group %s...\n", MULTICAST_GROUP);

  /**
   * Main receive loop: waits for multicast datagrams indefinitely.
   */
  while (1) {
    int n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &addr_len);

    if (n < 0) {
      perror("Receive failed");
    } else {
      buffer[n] = '\0';  // Null-terminate the received data
      printf("Received message: %s\n", buffer);
    }
  }

  /**
   * Cleanup (unreachable in current infinite-loop implementation).
   */
  close(sock);
  return 0;
}
