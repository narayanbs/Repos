/*
 * fileclient
 * Usage:
 * ./fileclient 127.0.0.1 test.txt output.txt
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFSIZE 4096
#define PORT 9090

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <server_ip> <remote_file> <local_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *server_ip = argv[1];
  const char *remote_file = argv[2];
  const char *local_file = argv[3];

  /* Create socket */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Server address */
  struct sockaddr_in servaddr = {.sin_family = AF_INET, .sin_port = htons(PORT)};

  if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
    perror("inet_pton");
    exit(EXIT_FAILURE);
  }

  /* Connect */
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  /* Send request */
  char request[256];
  snprintf(request, sizeof(request), "GET %s\n", remote_file);
  send(sockfd, request, strlen(request), 0);

  /* Receive file size */
  uint32_t net_size;
  ssize_t n = recv(sockfd, &net_size, sizeof(net_size), MSG_WAITALL);
  if (n <= 0) {
    printf("Server error or file not found\n");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  uint32_t file_size = ntohl(net_size);
  printf("Receiving %u bytes...\n", file_size);

  /* Open output file */
  FILE *fp = fopen(local_file, "wb");
  if (!fp) {
    perror("fopen");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  /* Receive file data */
  char buffer[BUFSIZE];
  uint32_t received = 0;

  while (received < file_size) {
    ssize_t bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    if (bytes <= 0) break;

    fwrite(buffer, 1, bytes, fp);
    received += bytes;
  }

  printf("Download complete (%u bytes)\n", received);

  fclose(fp);
  close(sockfd);
  return 0;
}
