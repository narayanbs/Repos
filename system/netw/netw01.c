// inet_ntop and inet_pton example
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

int main(void) {
  struct sockaddr_in6 sa;
  char str[INET6_ADDRSTRLEN];

  // store this IP address in sa:
  inet_pton(AF_INET6, "2001:db8:8714:3a90::12", &(sa.sin6_addr));

  // now get it back and print it
  inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);

  printf("%s\n", str);  // prints "2001:db8:8714:3a90::12"
}
