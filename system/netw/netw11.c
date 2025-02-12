/*
 * Multicast listener
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

int main() {
  int sock;
  struct sockaddr_in addr, src_addr;
  struct ip_mreq mreq;
  char buffer[BUFFER_SIZE];
  socklen_t addr_len = sizeof(struct sockaddr_in);

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  if (inet_pton(AF_INET, MULTICAST_GROUP, &(mreq.imr_multiaddr)) < 0) {
    perror("multicast group address creation failed");
    exit(EXIT_FAILURE);
  }

  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt failed");
    close(sock);
    exit(EXIT_FAILURE);
  }

  printf("Listening for messages on multicast group %s...\n", MULTICAST_GROUP);

  while (1) {
    int n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&src_addr, &addr_len);
    if (n < 0) {
      perror("Receive failed");
    } else {
      buffer[n] = '\0';
      printf("Received message: %s\n", buffer);
    }
  }
  close(sock);
  return 0;
}
