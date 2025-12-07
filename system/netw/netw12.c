/**
 * Simple Raw Socket Sniffer (TCP packets only)
 *
 * This program opens a raw socket and listens for all incoming TCP packets.
 * When a packet arrives, it extracts and prints basic information from the
 * IPv4 header (source address, destination address, total length, ID field).
 *
 * ⚠️ Requirements / Notes:
 *  - Must be run as root (raw sockets require elevated privileges).
 *  - Works only on Linux-like systems that support <netinet/ip.h>.
 *  - This code captures only TCP packets (IPPROTO_TCP).
 */

#include <arpa/inet.h>   // inet_ntoa, ntohs, htonl, sockaddr_in, etc.
#include <netinet/ip.h>  // struct iphdr definition
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main() {
  // sockaddr_in structures to hold the source and destination IP addresses
  struct sockaddr_in source_socket_address, dest_socket_address;

  int packet_size;

  // Allocate a buffer large enough for a full packet (max IPv4 size = 65535)
  unsigned char *buffer = (unsigned char *)malloc(65536);

  /**
   * Create a raw socket.
   *
   * PF_INET      → IPv4 protocol family
   * SOCK_RAW     → Raw socket (captures raw packets)
   * IPPROTO_TCP  → Capture only TCP packets
   *
   * Returns a file descriptor, or -1 on failure.
   */
  int sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sock == -1) {
    perror("Failed to create socket");
    exit(1);
  }

  // Capture packets indefinitely
  while (1) {
    /**
     * recvfrom()
     * Reads raw packet data from the socket into 'buffer'.
     *
     * buffer size: 65536 bytes
     * flags: 0 (no special options)
     * no need for sender address (NULL, NULL)
     */
    packet_size = recvfrom(sock, buffer, 65536, 0, NULL, NULL);
    if (packet_size == -1) {
      printf("Failed to get packets\n");
      return 1;
    }

    // Interpret the beginning of the buffer as an IPv4 header
    struct iphdr *ip_packet = (struct iphdr *)buffer;

    // Extract source IP address from the header
    memset(&source_socket_address, 0, sizeof(source_socket_address));
    source_socket_address.sin_addr.s_addr = ip_packet->saddr;

    // Extract destination IP address from the header
    memset(&dest_socket_address, 0, sizeof(dest_socket_address));
    dest_socket_address.sin_addr.s_addr = ip_packet->daddr;

    /**
     * Print packet information:
     * - Total packet length (in bytes)
     * - Source IP (converted from binary to dotted-decimal)
     * - Destination IP
     * - Identification field (used for fragmentation)
     */
    printf("Incoming Packet: \n");
    printf("Packet Size (bytes): %d\n", ntohs(ip_packet->tot_len));
    printf("Source Address: %s\n", inet_ntoa(source_socket_address.sin_addr));
    printf("Destination Address: %s\n", inet_ntoa(dest_socket_address.sin_addr));
    printf("Identification: %d\n\n", ntohs(ip_packet->id));
  }

  return 0;
}
