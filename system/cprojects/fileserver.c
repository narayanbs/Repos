/*
 * fileserver
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

#define BUFSIZE 4096
#define PORT 9090

/* Extract filename from request: "GET filename\n" */
bool get_filename(const char *request, char *filename, size_t size) {
  if (sscanf(request, "GET %255s", filename) == 1) return true;
  return false;
}

bool send_file(int sockfd, const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) return false;

  struct stat st;
  if (fstat(fd, &st) < 0) {
    close(fd);
    return false;
  }

  off_t offset = 0;
  uint32_t filesize = htonl(st.st_size);

  /* Send file size first */
  if (send(sockfd, &filesize, sizeof(filesize), 0) != sizeof(filesize)) {
    close(fd);
    return false;
  }

  /* Send file contents */
  while (offset < st.st_size) {
    ssize_t sent = sendfile(sockfd, fd, &offset, st.st_size - offset);
    if (sent <= 0) {
      close(fd);
      return false;
    }
  }

  close(fd);
  return true;
}

void *connection_handler(void *arg) {
  int sockfd = *(int *)arg;
  free(arg);

  char buffer[BUFSIZE];
  char filename[256];

  ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
  if (received <= 0) {
    close(sockfd);
    return NULL;
  }

  buffer[received] = '\0';

  if (!get_filename(buffer, filename, sizeof(filename))) {
    send(sockfd, "ERR\n", 4, 0);
    close(sockfd);
    return NULL;
  }

  if (!send_file(sockfd, filename)) {
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

  printf("Server listening on port %d\n", PORT);

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
    pthread_detach(tid);  // real concurrency
  }
}
