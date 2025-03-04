/*
 * Tun Interface
 * Do this before running the program
 *
 * sudo ip tuntap add dev tun0 mode tun
 * sudo ip addr add 10.0.0.1/24 dev tun0
 * sudo ip link set up dev tun0
 *
 * ip addr show
 * tun0: <NO-CARRIER,POINTOPOINT,MULTICAST,NOARP,UP> mtu 1500 qdisc fq_codel
 * state DOWN group default qlen 500 link/none inet 10.0.0.1/24 scope global
 * tun0 valid_lft forever preferred_lft forever
 *
 * Apart from the program, open two more windows
 * In one run
 * sudo tcpdump -i tun0 -v
 *
 * In another run
 * ping 10.0.0.2
 *
 * After running, clean up
 *
 * sudo ip link set down dev tun0
 * sudo ip addr del 10.0.0.1/24 dev tun0
 * sudo ip tuntap del dev tun0 mode tun
 * ip addr show
 */

#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IFNAMESIZ 16

int main() {
  int tunfd = open("/dev/net/tun", O_RDWR);
  struct ifreq ifr;
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strcpy(ifr.ifr_name, "tun0");
  ioctl(tunfd, TUNSETIFF, (void *)&ifr);

  while (1) {
    char buffer[1500];
    int nread = read(tunfd, buffer, sizeof(buffer));
    printf("Read %d bytes from device %s\n", nread, ifr.ifr_name);
  }
  close(tunfd);
  return 0;
}
