/*
 * Simple ICMP ping implementation (Linux)
 *
 * Requires:
 *   - root privileges or CAP_NET_RAW
 *
 * Build:
 *   gcc -Wall -Wextra -O2 netw14.c -o netw14
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

/* Configuration */
#define DEST_IP "8.8.8.8"
#define PAYLOAD_SIZE 56
#define RECVBUF_SIZE 1500
#define PING_INTERVAL 1000 /* ms */

/* Globals */
static volatile sig_atomic_t running = 1;

/* ---------------- Utility ---------------- */

static void handle_sigint(int sig) {
  (void)sig;
  running = 0;
}

/* Internet checksum */
static uint16_t checksum(const void *data, size_t len) {
  const uint16_t *buf = data;
  uint32_t sum = 0;

  while (len > 1) {
    sum += *buf++;
    len -= 2;
  }

  if (len == 1) {
    sum += *(const uint8_t *)buf;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);

  return (uint16_t)~sum;
}

/* Milliseconds difference */
static long diff_ms(const struct timespec *start, const struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000L + (end->tv_nsec - start->tv_nsec) / 1000000L;
}

/* ---------------- ICMP ---------------- */

static ssize_t build_echo_request(uint8_t *buf, size_t buflen, uint16_t id, uint16_t seq,
                                  struct timespec *send_time) {
  if (buflen < sizeof(struct icmphdr) + PAYLOAD_SIZE) {
    return -1;
  }

  struct icmphdr *icmp = (struct icmphdr *)buf;
  memset(icmp, 0, sizeof(*icmp) + PAYLOAD_SIZE);

  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = htons(id);
  icmp->un.echo.sequence = htons(seq);

  /* Store send timestamp in payload */
  memcpy(buf + sizeof(*icmp), send_time, sizeof(*send_time));

  icmp->checksum = checksum(buf, sizeof(*icmp) + PAYLOAD_SIZE);

  return sizeof(*icmp) + PAYLOAD_SIZE;
}

/* ---------------- Main ---------------- */

int main(void) {
  int sockfd;
  struct sockaddr_in dest = {0};
  uint8_t sendbuf[sizeof(struct icmphdr) + PAYLOAD_SIZE];
  uint8_t recvbuf[RECVBUF_SIZE];
  uint16_t seq = 0;
  uint16_t ident = (uint16_t)getpid();

  signal(SIGINT, handle_sigint);

  /* Destination */
  dest.sin_family = AF_INET;
  if (inet_pton(AF_INET, DEST_IP, &dest.sin_addr) != 1) {
    perror("inet_pton");
    return EXIT_FAILURE;
  }

  /* Raw socket */
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    if (errno == EPERM) {
      fprintf(stderr, "Error: raw sockets require root or CAP_NET_RAW\n");
    }
    perror("socket");
    return EXIT_FAILURE;
  }

  /* Set TTL */
  int ttl = 64;
  if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
    perror("setsockopt(IP_TTL)");
  }

  printf("PING %s (%s): %d bytes of data\n", DEST_IP, DEST_IP, PAYLOAD_SIZE);

  while (running) {
    struct timespec send_time, recv_time;

    clock_gettime(CLOCK_MONOTONIC, &send_time);

    ssize_t pktlen = build_echo_request(sendbuf, sizeof(sendbuf), ident, ++seq, &send_time);

    if (pktlen < 0) {
      fprintf(stderr, "Failed to build ICMP packet\n");
      break;
    }

    if (sendto(sockfd, sendbuf, pktlen, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
      perror("sendto");
      break;
    }

    struct pollfd pfd = {.fd = sockfd, .events = POLLIN};

    int ret = poll(&pfd, 1, PING_INTERVAL);
    if (ret == 0) {
      printf("Request timeout for icmp_seq %u\n", seq);
      continue;
    }
    if (ret < 0) {
      if (errno == EINTR) continue;
      perror("poll");
      break;
    }

    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    ssize_t n = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen);
    if (n < 0) {
      if (errno == EINTR) continue;
      perror("recvfrom");
      break;
    }

    clock_gettime(CLOCK_MONOTONIC, &recv_time);

    if ((size_t)n < sizeof(struct ip)) {
      continue;
    }

    struct ip *ip = (struct ip *)recvbuf;
    size_t iphdr_len = ip->ip_hl << 2;

    if ((size_t)n < iphdr_len + sizeof(struct icmphdr)) {
      continue;
    }

    struct icmphdr *icmp = (struct icmphdr *)(recvbuf + iphdr_len);

    if (icmp->type != ICMP_ECHOREPLY || icmp->un.echo.id != htons(ident)) {
      continue;
    }

    struct timespec *sent = (struct timespec *)(recvbuf + iphdr_len + sizeof(*icmp));

    long rtt = diff_ms(sent, &recv_time);

    char addrbuf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &from.sin_addr, addrbuf, sizeof(addrbuf));

    printf("%zu bytes from %s: icmp_seq=%u ttl=%d time=%ld ms\n", n - iphdr_len, addrbuf,
           ntohs(icmp->un.echo.sequence), ip->ip_ttl, rtt);
  }

  close(sockfd);
  puts("\nPing terminated.");
  return EXIT_SUCCESS;
}
