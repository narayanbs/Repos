/*
 * fileclient with resume and progress bar
 * Usage:
 * Get <filename> offset
 * Ex:
 * GET bigfile.iso 10485760
 */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFSIZE 4096
#define PORT 9090

off_t get_local_size(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == 0) return st.st_size;
  return 0;
}

void show_progress(off_t received, off_t total) {
  int percent = (int)((received * 100) / total);
  printf("\rProgress: %3d%% (%ld / %ld bytes)", percent, received, total);
  fflush(stdout);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: %s <server_ip> <remote_file> <local_file>\n", argv[0]);
    return 1;
  }

  const char *server_ip = argv[1];
  const char *remote = argv[2];
  const char *local = argv[3];

  off_t offset = get_local_size(local);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};
  inet_pton(AF_INET, server_ip, &addr.sin_addr);

  connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

  /* Send request with offset */
  char request[512];
  snprintf(request, sizeof(request), "GET %s %ld\n", remote, offset);
  send(sockfd, request, strlen(request), 0);

  /* Receive remaining size */
  uint32_t net_size;
  recv(sockfd, &net_size, sizeof(net_size), MSG_WAITALL);
  off_t remaining = ntohl(net_size);
  off_t total = offset + remaining;

  FILE *fp = fopen(local, offset ? "ab" : "wb");
  if (!fp) {
    perror("fopen");
    return 1;
  }

  char buffer[BUFSIZE];
  off_t received = offset;

  while (remaining > 0) {
    ssize_t n = recv(sockfd, buffer, BUFSIZE, 0);
    if (n <= 0) break;

    fwrite(buffer, 1, n, fp);
    remaining -= n;
    received += n;

    show_progress(received, total);
  }

  printf("\nDownload complete.\n");

  fclose(fp);
  close(sockfd);
  return 0;
}
