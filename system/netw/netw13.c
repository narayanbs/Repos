/*
This program has to be run in root. and pass an IP instead of a hostname
sudo ./netw13 142.98.23.3 
Make sure you comment line 364 setsockopt( ... SO_DONTROUTE) 

This program creates a socket interface with protocol field set to "icmp". Now,  frame an ICMP
header with packet type as "ICMP echo". This icmp header is attached to L3 packet in kernel network
stack and now, the ICMP echo (ping) packets is sent to the destination host ip. On receiving ICMP
echo packet, the destination host (if reachable) would send "ICMP reply" packet back to the source
host. The below program on receiving ICMP reply would parse the ICMP header and validate whether the
ICMP header is a valid "ICMP reply" header. The program runs in infinite while loop and this process
goes on forever, until "Ctrl + c" is pressed.

On pressing "ctrl + C", the program will invoke SIGINT signal handler code and print the statistics
of the ping (i.e. no. of ICMP echo packets transmitted and the no. of ICMP reply packets received)
and the program exits following this.

Please refer the following rfc link pertaining to "Internet Control Message Protocol"
https://tools.ietf.org/html/rfc792

Program
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PACKET_SIZE 4096
#define ICMP_DATA_LEN 56

/* Store sent and received packet in the below arrays */
char sendpacket[PACKET_SIZE];  // store the send package
char recvpacket[PACKET_SIZE];  // store the recevice package

int sockfd;
int nsend = 0, nreceived = 0;

/* Destination and local host address info */
struct sockaddr_in dest_addr;  // store the destination address info
struct sockaddr_in from;       // store the localhost address info

struct timeval tm_recv;  // store the time info when a package received
pid_t pid;               // store the process id of main program
char *hostname = NULL;   // store the host name (from the command line)

/******************************************************************************
 * Function Declarations
 ******************************************************************************/
void signal_handler(int signo);
unsigned short cal_chksum(unsigned short *addr, int len);
int frame_icmp_echo_header(int pack_no);
int unpack_icmp_reply(char *buf, int len);
void send_icmp_echo_packet(void);
void recv_icmp_reply_packet(void);
void time_difference_in_secs(struct timeval *time_recv, struct timeval *time_send);

/*******************************************************************
 * Function: signal_handler
 *
 * Description: SIGINT signal handler. When 'ctrl + c' is pressed on
 *              keyboard, signal handler prints the summary of ping
 *              packets transmitted and received.
 *******************************************************************/
void signal_handler(int signo) {
  putchar('\n');
  fflush(stdout);
  printf("\n-----------%s PING statistics-----------\n", hostname);
  if (nsend > 0) {
    printf("%d packets transmitted, %d received , %2.0f%%  lost\n", nsend, nreceived,
           (float)(nsend - nreceived) / nsend * 100);
  } else {
    printf("have problem in send packets!\n");
  }

  if (sockfd) {
    close(sockfd);
  }
  exit(0);
}

/*******************************************************************
 * Function: unpack_icmp_reply
 *
 * Description: Extract icmp header from icmp reply packet
 *
 * Arguments:
 *      buf : Pointer to received ICMP reply packet packet array.
 *      len : Length of the ICMP reply packet received.
 *******************************************************************/
int unpack_icmp_reply(char *buf, int len) {
  int iphdrlen;
  struct ip *ip;
  struct icmp *icmp;
  struct timeval *tm_send;
  double rtt;

  ip = (struct ip *)buf;

  /* Seek the ip header length, which is the length indicator of
   * ip header multiplied by 4
   * The header length indicates the number of 4-byte words contained
   * in the header.
   * The acceptable minimum is 5 and the maximum is 15
   */
  iphdrlen = ip->ip_hl << 2;

  /* Beyond the ip header, point to the ICMP header */
  icmp = (struct icmp *)(buf + iphdrlen);
  /* Total length of ICMP header and ICMP datagram*/
  len -= iphdrlen;

  /*
   * Less than ICMP header length is unreasonable
   */
  if (len < 8) {
    printf("ICMP packets\'s length is less than 8\n");
    return -1;
  }

  /*
   * Be sure to receive a response to a previously issued ICMP packet
   * icmpid is the same as the PID of the process which has initiated the
   * ICMP ping (i.e. ICMP echo) packet in first place.
   */
  if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid)) {
    tm_send = (struct timeval *)icmp->icmp_data;

    /*Received and sent time difference*/
    time_difference_in_secs(&tm_recv, tm_send);

    /* Calculate rtt in milliseconds */
    rtt = tm_recv.tv_sec * 1000 + tm_recv.tv_usec / 1000;

    /* Display related information */
    printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n", len, inet_ntoa(from.sin_addr),
           icmp->icmp_seq, ip->ip_ttl, rtt);
  } else
    return -1;

  return 0;
}

/*******************************************************************
 *  Function: time_difference_in_secs
 *
 *  Description: The time difference is calculated by subtracting
 *  two timeval structures
 *******************************************************************/
void time_difference_in_secs(struct timeval *time_recv, struct timeval *time_send) {
  /*
   * For sure recv secs will be greater than send secs. But, what
   * if recv usecs is less than send usecs
   */
  if ((time_recv->tv_usec -= time_send->tv_usec) < 0) {
    --time_recv->tv_sec;
    time_recv->tv_usec += 1000000;
  }
  time_recv->tv_sec -= time_send->tv_sec;
}

/*******************************************************************
 * Function: cal_chksum
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
unsigned short cal_chksum(unsigned short *addr, int len) {
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;

  /*
   * The checksum is the 16-bit ones's complement of the one's
   * complement sum of the ICMP message starting with the ICMP Type.
   * For computing the checksum , the checksum field should be zero.
   */

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(unsigned char *)(&answer) = *(unsigned char *)w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return answer;
}

/*******************************************************************
 * Function: frame_icmp_echo_header
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
int frame_icmp_echo_header(int pack_no) {
  int packsize;
  struct icmp *icmp;
  struct timeval *tval;

  /* ICMP Header structure */
  icmp = (struct icmp *)sendpacket;
  /* ICMP echo TYPE */
  icmp->icmp_type = ICMP_ECHO;  // 8 : Echo Request
  icmp->icmp_code = 0;          // 0 = net unreachable
  icmp->icmp_cksum = 0;
  icmp->icmp_seq = pack_no;
  icmp->icmp_id = pid;
  packsize = 8 + ICMP_DATA_LEN;  // 8 + 56 (data) = 64 Bytes ICMP header
  tval = (struct timeval *)icmp->icmp_data;
  gettimeofday(tval, NULL);
  /* Calculate checksum */
  icmp->icmp_cksum = cal_chksum((unsigned short *)icmp, packsize);
  return packsize;
}

/*******************************************************************
 * Function: send_icmp_echo_packet
 *
 * Description: Send "ICMP echo" packet to Destination Host
 *******************************************************************/
void send_icmp_echo_packet() {
  int packetsize;

  nsend++;
  /* ICMP EHCO PACKET HEADER */
  packetsize = frame_icmp_echo_header(nsend);

  /* Send the ICMP packet to the destination Address */
  if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <
      0) {
    perror("sendto error");
    nsend--;
  }
  sleep(1);
}

/*******************************************************************
 * Function: recv_icmp_reply_packet
 *
 * Description: Receive "ICMP reply" packet from Destination Host
 *******************************************************************/
void recv_icmp_reply_packet() {
  unsigned int n, fromlen;
  extern int errno;

  signal(SIGALRM, signal_handler);

  fromlen = sizeof(from);

  /* Received packet is stored in recvpacket global array */
  while (nreceived < nsend) {
    if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)&from,
                      &fromlen)) < 0) {
      if (errno == EINTR) continue;

      perror("recvfrom error");
      continue;
    }

    /* Get the current time stamp of the received ICMP reply packet */
    gettimeofday(&tm_recv, NULL);

    /* Parse through received icmp reply packet */
    if (unpack_icmp_reply(recvpacket, n) == -1) {
      continue;
    }
    nreceived++;
  }
}

/********************************************************************************
 *                                main
 ********************************************************************************/
int main(int argc, char *argv[]) {
  struct protoent *protocol;

  int size = 50 * 1024;
  int on = 1;

  /* Register the process for SIGINT and SIGALRM */
  signal(SIGINT, signal_handler);
  signal(SIGALRM, signal_handler);

  if (argc != 2) {
    printf("Enter Destination Host address as 1st argument\n");
  }

  /* IP address (x.y.z.a) of the destination host: Global variable */
  hostname = argv[1];

  /* Protocol: ICMP */
  if ((protocol = getprotobyname("icmp")) == NULL) {
    perror("getprotobyname");
    exit(1);
  }

  /* Create a RAW socket interface for ICMP echo packet. Socket file descriptor
   * sockfd is a global variable
   */
  if ((sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto)) < 0) {
    perror("socket error");
    exit(1);
  }

  /* SO_RCVBUF
   * Sets or gets the maximum socket receive buffer in bytes. The
   * kernel doubles this value (to allow space for bookkeeping
   * overhead) when it is set using setsockopt(2), and this doubled
   * value is returned by getsockopt(2).
   */
  /* Manipulating socket options.  */
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
  /*
   * SO_DONTROUTE
   * Don't send via a gateway, send only to directly connected
   * hosts.
   */
  /*************************************************************************
   * IN ORDER TO PING TO NETWORKS BEYOND THE GATEWAY, COMMENT OUT THE BELOW
   * LINE.
   *************************************************************************/
  setsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE, &on, sizeof(on));

  bzero(&dest_addr, sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  /* The  inet_addr() function converts the Internet host address
   * from IPv4 numbers-and-dots notation (x.y.z.a) into binary data in network
   * byte order.
   */
  dest_addr.sin_addr.s_addr = inet_addr(hostname);

  pid = getpid();
  /* The inet_ntoa() function converts the Internet host address in,
   * given in network byte order, to a string in IPv4 dotted-decimal
   * notation (x.y.z.a).
   */
  printf("PING %s(%s): %d bytes data in ICMP packets.\n", hostname, inet_ntoa(dest_addr.sin_addr),
         ICMP_DATA_LEN);

  for (;;) {
    /* ICMP echo Ping */
    send_icmp_echo_packet();
    /* ICMP reply */
    recv_icmp_reply_packet();
  }

  return 0;
}
