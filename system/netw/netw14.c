#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

// Pseudo header needed for checksum calculation
struct pseudo_header {
  u_int32_t source_address;
  u_int32_t dest_address;
  u_int8_t placeholder;
  u_int8_t protocol;
  u_int16_t tcp_length;
};

// Checksum function
unsigned short checksum(void *b, int len) {
  unsigned short *buf = b;
  unsigned int sum = 0;
  unsigned short result;

  for (sum = 0; len > 1; len -= 2) {
    sum += *buf++;
  }

  if (len == 1) {
    sum += *(unsigned char *)buf;
  }

  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;

  return result;
}

int main() {
  int sockfd;
  struct sockaddr_in dest_addr;
  struct icmp header;
  char packet[4096];

  // Fill in the destination information
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = 0;
  dest_addr.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google's public DNS server

  // Create a raw socket
  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0) {
    perror("Socket creation failed");
    exit(1);
  }

  // Loop to send pings and receive responses
  while (1) {
    // Initialize the ICMP header
    memset(&header, 0, sizeof(struct icmp));
    header.icmp_type = ICMP_ECHO; // Echo Request
    header.icmp_code = 0;
    header.icmp_id = getpid(); // Set the process ID as the identifier
    header.icmp_seq++;         // Increment sequence number for each request

    // Calculate the checksum for the ICMP header
    header.icmp_cksum = checksum(&header, sizeof(struct icmp));

    // Set up the pseudo header for checksum calculation
    struct pseudo_header psh;
    memset(&psh, 0, sizeof(struct pseudo_header));
    psh.source_address = inet_addr("192.168.1.1"); // Your source address
    psh.dest_address = dest_addr.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_ICMP;
    psh.tcp_length = htons(sizeof(struct icmp));

    int psize = sizeof(struct pseudo_header) + sizeof(struct icmp);
    char *pseudogram = (char *)malloc(psize);

    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), &header, sizeof(struct icmp));

    // Calculate the checksum for the entire packet
    header.icmp_cksum = checksum((unsigned short *)pseudogram, psize);

    // Send the packet
    if (sendto(sockfd, &header, sizeof(struct icmp), 0, (struct sockaddr *)&dest_addr,
               sizeof(dest_addr)) <= 0) {
      perror("Send failed");
      close(sockfd);
      exit(1);
    } else {
      printf("Sent ICMP Echo Request, Seq: %d\n", header.icmp_seq);
    }

    // Wait for the reply (ICMP Echo Reply)
    struct sockaddr_in r_addr;
    socklen_t addr_len = sizeof(r_addr);
    char buffer[1024];

    // Record the time before waiting for reply
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int bytes_received =
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&r_addr, &addr_len);
    if (bytes_received < 0) {
      perror("Recv failed");
      close(sockfd);
      exit(1);
    }

    // Record the time after receiving the reply
    gettimeofday(&end_time, NULL);

    // Parse the IP header
    struct ip *ip_header = (struct ip *)buffer;

    // Parse the ICMP header (skip the IP header)
    struct icmp *icmp_header = (struct icmp *)(buffer + (ip_header->ip_hl << 2));

    // Calculate round-trip time (RTT)
    long rtt = (end_time.tv_sec - start_time.tv_sec) * 1000 +
               (end_time.tv_usec - start_time.tv_usec) / 1000;

    // Check if it's an Echo Reply (ICMP type 0)
    if (icmp_header->icmp_type == ICMP_ECHOREPLY) {
      printf("Received ICMP Echo Reply from %s, Seq: %d, RTT: %ld ms\n", inet_ntoa(r_addr.sin_addr),
             icmp_header->icmp_seq, rtt);
    } else {
      printf("Received non-echo reply\n");
    }

    // Sleep for a short period before sending the next ping
    usleep(1000000); // Sleep for 1 second (1000000 microseconds)
  }

  close(sockfd);
  return 0;
}

