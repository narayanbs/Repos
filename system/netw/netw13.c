/*
 * Program
 * Simple ICMP Ping implementation using raw sockets (Linux)
 *
 * This program has to be run in root. and pass an IP instead of a hostname
 *
 * - Uses IPPROTO_ICMP raw socket (root required)
 * - Sends ICMP Echo Requests
 * - Receives ICMP Echo Replies
 * - Measures RTT using embedded timestamps
 *
 * Linux-correct:
 *   - struct iphdr     (not struct ip)
 *   - struct icmphdr   (not struct icmp)
 *
 * Make sure you comment line 364 setsockopt( ... SO_DONTROUTE)
 * 
 * This program creates a socket interface with protocol field set to "icmp". Now,  frame an ICMP
 * header with packet type as "ICMP echo". This icmp header is attached to L3 packet in kernel network
 * stack and now, the ICMP echo (ping) packets is sent to the destination host ip. On receiving ICMP
 * echo packet, the destination host (if reachable) would send "ICMP reply" packet back to the source
 * host. The below program on receiving ICMP reply would parse the ICMP header and validate whether the
 * ICMP header is a valid "ICMP reply" header. The program runs in infinite while loop and this process
 * goes on forever, until "Ctrl + c" is pressed.
 * 
 * On pressing "ctrl + C", the program will invoke SIGINT signal handler code and print the statistics
 * of the ping (i.e. no. of ICMP echo packets transmitted and the no. of ICMP reply packets received)
 * and the program exits following this.
 * 
 * Please refer the following rfc link pertaining to "Internet Control Message Protocol"
 * https://tools.ietf.org/html/rfc792
 * 
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -pedantic netw13.c -o netw13
 *
 * Run:
 *   sudo ./netw13 <IPv4-address>
 *   sudo ./netw13 142.98.23.3
 */

#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>       // struct iphdr
#include <netinet/ip_icmp.h>  // struct icmphdr
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/* Maximum packet buffer size */
#define PACKET_SIZE 4096

/* Payload size carried in ICMP echo request */
#define ICMP_DATA_LEN 56

/* Buffers for sending and receiving packets */
static char sendpacket[PACKET_SIZE];
static char recvpacket[PACKET_SIZE];

/* Raw socket descriptor */
static int sockfd = -1;

/* Packet counters (atomic for signal safety) */
static volatile sig_atomic_t nsend = 0;
static volatile sig_atomic_t nreceived = 0;

/* Destination and source address structures */
static struct sockaddr_in dest_addr;
static struct sockaddr_in from;

/* Time when a reply is received */
static struct timeval tm_recv;

/* Process ID used as ICMP identifier */
static pid_t pid;

/* Destination IP string */
static const char *hostname;

/* ============================================================
 * Internet checksum calculation (RFC 1071)
 * ============================================================
 *
 * Description: calculate checksum
 *
 *    ################################################################
 *    ###################  ICMP   HEADER #############################
 *    ################################################################
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             unused                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |      Internet Header + 64 bits of Original Data Datagram      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Arguments: 1st argument: pointer to beginning of the ICMP header
 *            2nd argument: Length of ICMP packet header (64 bytes)
 *******************************************************************/
static unsigned short checksum(void *buf, int len) {
  unsigned int sum = 0;
  unsigned short *ptr = buf;

  /* Sum 16-bit words */
  while (len > 1) {
    sum += *ptr++;
    len -= 2;
  }

  /* Handle trailing byte, if any */
  if (len == 1) {
    sum += *(unsigned char *)ptr;
  }

  /* Fold 32-bit sum to 16 bits */
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);

  return (unsigned short)~sum;
}

/* ============================================================
 * Calculate recv_time - send_time
 * ============================================================ */
static void time_diff(struct timeval *recv, const struct timeval *send) {
  recv->tv_sec -= send->tv_sec;
  recv->tv_usec -= send->tv_usec;

  if (recv->tv_usec < 0) {
    recv->tv_sec--;
    recv->tv_usec += 1000000;
  }
}

/* ============================================================
 * Signal handler (SIGINT, SIGALRM)
 * Prints statistics and exits
 * ============================================================ */
static void signal_handler(int signo) {
  (void)signo;

  /* Async-signal-safe newline */
  write(STDOUT_FILENO, "\n", 1);

  printf("\n--- %s ping statistics ---\n", hostname);
  if (nsend > 0) {
    printf("%d packets transmitted, %d received, %.0f%% packet loss\n", nsend, nreceived,
           ((double)(nsend - nreceived) / nsend) * 100.0);
  }

  if (sockfd >= 0) close(sockfd);

  _exit(0);
}

/* Register a signal using sigaction (modern, reliable) */
static void setup_signal(int sig) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(sig, &sa, NULL) < 0) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
}

/* ============================================================
 * Build ICMP Echo Request packet
 * ============================================================
 *
 * Description: Send "ICMP echo" packet to Destination Host
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             unused                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |      Internet Header + 64 bits of Original Data Datagram      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *******************************************************************/
static int build_icmp_echo(int seq) {
  struct icmphdr *icmp = (struct icmphdr *)sendpacket;

  /* Fill ICMP header */
  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->checksum = 0;
  icmp->un.echo.id = pid;
  icmp->un.echo.sequence = seq;

  /* Store send timestamp in payload */
  struct timeval *tv = (struct timeval *)(sendpacket + sizeof(struct icmphdr));
  gettimeofday(tv, NULL);

  /* ICMP header + payload */
  int size = sizeof(struct icmphdr) + ICMP_DATA_LEN;

  /* Compute checksum */
  icmp->checksum = checksum(sendpacket, size);

  return size;
}

/* ============================================================
 * Send ICMP Echo Request
 * ============================================================ */
static void send_echo(void) {
  int size = build_icmp_echo(++nsend);

  if (sendto(sockfd, sendpacket, size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
    perror("sendto");
    nsend--;
  }

  /* Send one packet per second */
  sleep(1);
}

/* ============================================================
 * Parse ICMP Echo Reply
 * ============================================================ */
static int parse_reply(char *buf, int len) {
  /* IP header is filled by kernel */
  struct iphdr *ip = (struct iphdr *)buf;

  /* IHL is in 32-bit words */
  int iphdrlen = ip->ihl * 4;

  if (len < iphdrlen + (int)sizeof(struct icmphdr)) return -1;

  struct icmphdr *icmp = (struct icmphdr *)(buf + iphdrlen);

  /* Validate Echo Reply and process ID */
  if (icmp->type == ICMP_ECHOREPLY && icmp->un.echo.id == pid) {
    /* Extract send timestamp */
    struct timeval *sent = (struct timeval *)(buf + iphdrlen + sizeof(struct icmphdr));

    /* Compute RTT */
    time_diff(&tm_recv, sent);

    double rtt = tm_recv.tv_sec * 1000.0 + tm_recv.tv_usec / 1000.0;

    printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n", len - iphdrlen,
           inet_ntoa(from.sin_addr), icmp->un.echo.sequence, ip->ttl, rtt);

    return 0;
  }

  return -1;
}

/* ============================================================
 * Receive ICMP Echo Replies
 * ============================================================ */
static void receive_reply(void) {
  socklen_t fromlen = sizeof(from);

  while (nreceived < nsend) {
    ssize_t n =
        recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from, &fromlen);

    if (n < 0) {
      if (errno == EINTR) continue;
      perror("recvfrom");
      continue;
    }

    gettimeofday(&tm_recv, NULL);

    if (parse_reply(recvpacket, (int)n) == 0) nreceived++;
  }
}

/* ============================================================
 * main
 * ============================================================ */
int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <IPv4 address>\n", argv[0]);
    return EXIT_FAILURE;
  }

  hostname = argv[1];
  pid = getpid();

  /* Register signal handlers */
  setup_signal(SIGINT);
  setup_signal(SIGALRM);

  /* Create raw ICMP socket (root required) */
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  /* Increase receive buffer */
  int rcvbuf = 50 * 1024;

  /* SO_RCVBUF
   * Sets or gets the maximum socket receive buffer in bytes. The
   * kernel doubles this value (to allow space for bookkeeping
   * overhead) when it is set using setsockopt(2), and this doubled
   * value is returned by getsockopt(2).
   */
  /* Manipulating socket options.  */
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));
  /*
   * SO_DONTROUTE
   * Don't send via a gateway, send only to directly connected
   * hosts.
   */
  /*************************************************************************
   * IN ORDER TO PREVENT PING TO NETWORKS BEYOND THE GATEWAY, UNCOMMENT THE BELOW
   * LINE.
   *************************************************************************/
  // int on = 1;
  // setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on));

  /* Destination address setup */
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;

  if (inet_pton(AF_INET, hostname, &dest_addr.sin_addr) != 1) {
    fprintf(stderr, "Invalid IPv4 address\n");
    return EXIT_FAILURE;
  }

  printf("PING %s (%s): %d bytes of data.\n", hostname, inet_ntoa(dest_addr.sin_addr),
         ICMP_DATA_LEN);

  /* Main loop */
  for (;;) {
    send_echo();
    receive_reply();
  }
}
