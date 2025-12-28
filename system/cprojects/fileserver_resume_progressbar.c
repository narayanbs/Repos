/*
 * fileserver that supports resume and showing progress bar in client
 * usage: GET filename offset\n
 * Ex:
 * GET video.mp4 0
 * GET big.iso 10485760
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define PORT 9090
#define BUFSIZE 4096

/* Send file starting from offset */
bool send_file(int sockfd, const char *filename, off_t offset) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) return false;

  struct stat st;
  if (fstat(fd, &st) < 0) {
    close(fd);
    return false;
  }

  if (offset > st.st_size) offset = 0;

  off_t remaining = st.st_size - offset;
  uint32_t net_size = htonl(remaining);

  /* Send remaining size */
  if (send(sockfd, &net_size, sizeof(net_size), 0) != sizeof(net_size)) {
    close(fd);
    return false;
  }

  /* Move file pointer */
  if (lseek(fd, offset, SEEK_SET) < 0) {
    close(fd);
    return false;
  }

  off_t sent = 0;
  while (sent < remaining) {
    ssize_t n = sendfile(sockfd, fd, &offset, remaining - sent);
    if (n <= 0) break;
    sent += n;
  }

  close(fd);
  return sent == remaining;
}

/* Thread handler */
void *connection_handler(void *arg) {
  int sockfd = *(int *)arg;
  free(arg);

  char buffer[BUFSIZE];
  char filename[256];
  off_t offset = 0;

  ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
  if (n <= 0) {
    close(sockfd);
    return NULL;
  }

  buffer[n] = '\0';

  /* Parse: GET filename offset */
  if (sscanf(buffer, "GET %255s %ld", filename, &offset) < 1) {
    send(sockfd, "ERR\n", 4, 0);
    close(sockfd);
    return NULL;
  }

  if (!send_file(sockfd, filename, offset)) {
    send(sockfd, "NO\n", 3, 0);
  }

  close(sockfd);
  return NULL;
}

int main(void) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_port = htons(PORT), .sin_addr.s_addr = INADDR_ANY};

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  printf("File server listening on port %d\n", PORT);

  while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }

    int *pclient = malloc(sizeof(int));
    *pclient = client_fd;

    pthread_t tid;
    pthread_create(&tid, NULL, connection_handler, pclient);
    pthread_detach(tid);  // allow multiple clients
  }
}
