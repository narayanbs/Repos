/**
 * @file multicast_sender.c
 * @brief A simple multicast UDP sender example in C.
 *
 * This program creates a UDP socket, configures the multicast
 * TTL (time-to-live), and repeatedly sends a test message to a
 * multicast group address at fixed intervals.
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

#define MSG "Hello, Multicast group"
#define MULTICAST_GROUP "239.255.255.250"
#define PORT 12345
#define TTL_VALUE 64

/**
 * @brief Program entry point.
 *
 * Creates a UDP socket, sets the multicast TTL option,
 * prepares the multicast destination address, and repeatedly
 * sends a text message every 2 seconds.
 *
 * @return int Exit status: 0 on success, nonzero on error.
 */
int main() {
  int sock;
  struct sockaddr_in addr;
  int ttl = TTL_VALUE;

  /**
   * Create UDP socket for sending datagrams.
   */
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  /**
   * Set multicast TTL (time-to-live).
   * Controls how many router hops the packet can cross.
   */
  if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
    perror("setting ttl failed");
    exit(EXIT_FAILURE);
  }

  /**
   * Initialize multicast destination address structure.
   */
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);

  /**
   * Convert multicast group string to binary network format.
   */
  if (inet_pton(AF_INET, MULTICAST_GROUP, &(addr.sin_addr)) < 0) {
    perror("Error creating address");
    exit(EXIT_FAILURE);
  }

  /**
   * Main loop: send the multicast message every 2 seconds.
   */
  while (1) {
    if (sendto(sock, MSG, strlen(MSG), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("Send failed");
    } else {
      printf("Message sent: %s\n", MSG);
    }

    sleep(2);  // Delay between messages
  }

  /**
   * Cleanup (unreachable because of infinite loop).
   */
  close(sock);
  return 0;
}
