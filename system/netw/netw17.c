/*
 * A minimal HTTPS server that uses openssl for TLS
 */
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 4096

int main(void) {
  /* ---------- Initialize OpenSSL ---------- */
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  /* Create SSL context for a TLS server */
  SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
  if (!ctx) {
    ERR_print_errors_fp(stderr);
    return EXIT_FAILURE;
  }

  /* Load certificate and private key */
  if (SSL_CTX_use_certificate_chain_file(ctx, "fullchain") <= 0 ||
      SSL_CTX_use_PrivateKey_file(ctx, "thekey", SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* ---------- Create TCP socket ---------- */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* Allow fast restart */
  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  /* Server address */
  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  /* Bind socket */
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    close(sockfd);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* Listen for connections */
  if (listen(sockfd, 10) < 0) {
    perror("listen");
    close(sockfd);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  printf("HTTPS server listening on port %d\n", PORT);

  /* ---------- Accept one client ---------- */
  int clientfd = accept(sockfd, NULL, NULL);
  if (clientfd < 0) {
    perror("accept");
    close(sockfd);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* ---------- TLS handshake ---------- */
  SSL *ssl = SSL_new(ctx);
  SSL_set_fd(ssl, clientfd);

  if (SSL_accept(ssl) <= 0) {
    ERR_print_errors_fp(stderr);
    SSL_free(ssl);
    close(clientfd);
    close(sockfd);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* ---------- Read HTTP request ---------- */
  char buffer[BUF_SIZE] = {0};
  int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
  if (bytes <= 0) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(clientfd);
    close(sockfd);
    SSL_CTX_free(ctx);
    return EXIT_FAILURE;
  }

  /* ---------- Very basic request parsing ---------- */
  const char *body;
  FILE *file = NULL;

  if (strncmp(buffer, "GET /index.html", 15) == 0 || strncmp(buffer, "GET / ", 6) == 0) {
    file = fopen("index.html", "r");
  }

  char response[BUF_SIZE] = {0};

  if (file) {
    const char *header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";

    strcpy(response, header);
    fread(response + strlen(header), 1, sizeof(response) - strlen(header) - 1, file);
    fclose(file);
  } else {
    body =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "404 - Page not found";
    strcpy(response, body);
  }

  /* ---------- Send response ---------- */
  SSL_write(ssl, response, strlen(response));

  /* ---------- Cleanup ---------- */
  SSL_shutdown(ssl);
  SSL_free(ssl);
  close(clientfd);
  close(sockfd);
  SSL_CTX_free(ctx);

  return 0;
}
