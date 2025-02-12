/*
 * Multicast sender
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

int main() {
  int sock;
  struct sockaddr_in addr;
  struct ip_mreq mreq;
  int ttl = TTL_VALUE;

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
    perror("setting ttl failed");
    exit(EXIT_FAILURE);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, MULTICAST_GROUP, &(addr.sin_addr)) < 0) {
    perror("Error creating address");
    exit(EXIT_FAILURE);
  }

  while (1) {
    if (sendto(sock, MSG, strlen(MSG), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("Send failed");
    } else {
      printf("Message sent: %s\n", MSG);
    }
    sleep(2);
  }

  close(sock);
  return 0;
}
