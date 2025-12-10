/*
** epollserver.c -- a cheezy multiperson chat server
*/

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "9034"  // port we're listening on

#define MAXFDS 16 * 1024

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// Add a new file descriptor to the set
void add_to_fds(int *fds[], int newfd, int *fd_count, int *fd_size) {
  // If we don't have room, add more space in the pfds array
  if (*fd_count == *fd_size) {
    *fd_size *= 2;  // Double it

    *fds = realloc(*fds, sizeof(**fds) * (*fd_size));
  }
  (*fds)[*fd_count] = newfd;

  (*fd_count)++;
}

// Remove an index from the set
void del_from_fds(int *fds[], int fd, int *fd_count) {
  for (int i = 0; i < *fd_count; i++) {
    if ((*fds)[i] == fd) {
      // move last element here
      (*fds)[i] = (*fds)[*fd_count - 1];
      (*fd_count)--;
      return;
    }
  }
}

int main(void) {
  setvbuf(stdout, NULL, _IONBF, 0);

  int *fds = malloc(sizeof(int) * MAXFDS);
  int fd_count = 0;
  int fd_size = MAXFDS;

  int listener;                        // listening socket descriptor
  int newfd;                           // newly accept()ed socket descriptor
  struct sockaddr_storage remoteaddr;  // client address
  socklen_t addrlen;

  char buf[256];  // buffer for client data
  int nbytes;

  char remoteIP[INET6_ADDRSTRLEN];

  int yes = 1;  // for setsockopt() SO_REUSEADDR, below
  int i, j, rv;

  struct addrinfo hints, *ai, *p;

  // get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    // lose the pesky "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }

    break;
  }

  // if we got here, it means we didn't get bound
  if (p == NULL) {
    fprintf(stderr, "epoll: failed to bind\n");
    exit(2);
  }

  freeaddrinfo(ai);  // all done with this

  // listen
  if (listen(listener, 10) == -1) {
    perror("listen");
    exit(3);
  }

  int flags = fcntl(listener, F_GETFL, 0);
  if (flags == -1) {
    perror("getfl failed");
    exit(EXIT_FAILURE);
  }
  fcntl(listener, F_SETFL, flags | O_NONBLOCK);

  int epollfd = epoll_create1(0);
  if (epollfd < 0) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  struct epoll_event accept_event;
  accept_event.data.fd = listener;
  accept_event.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &accept_event) < 0) {
    perror("epoll_ctl EPOLL_CTL_ADD");
    exit(EXIT_FAILURE);
  }

  struct epoll_event *events = calloc(MAXFDS, sizeof(struct epoll_event));
  if (events == NULL) {
    perror("unable to allocate memory for epoll_events");
    exit(EXIT_FAILURE);
  }

  // main loop
  for (;;) {
    int nready = epoll_wait(epollfd, events, MAXFDS, -1);

    // run through the existing connections looking for data to read
    for (i = 0; i < nready; i++) {
      if (events[i].events & EPOLLERR) {
        perror("epoll_wait returned EPOLLERR");
        exit(EXIT_FAILURE);
      }
      if (events[i].data.fd == listener) {
        addrlen = sizeof remoteaddr;
        newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

        if (newfd < 0) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // This can happen due to the nonblocking socket mode; in this
            // case don't do anything, but print a notice (since these events
            // are extremely rare and interesting to observe...)
            printf("accept returned EAGAIN or EWOULDBLOCK\n");
          } else {
            perror("accept");
            exit(1);
          }
        } else {
          printf(
              "epoll: new connection from %s on "
              "socket %d\n",
              inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP,
                        INET6_ADDRSTRLEN),
              newfd);

          int flags = fcntl(newfd, F_GETFL, 0);
          if (flags == -1) {
            perror("getfl failed");
            exit(EXIT_FAILURE);
          }
          fcntl(newfd, F_SETFL, flags | O_NONBLOCK);

          struct epoll_event event = {0};
          event.data.fd = newfd;
          event.events |= EPOLLIN | EPOLLET;

          if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newfd, &event) < 0) {
            perror("epoll_ctl EPOLL_CTL_ADD");
            exit(EXIT_FAILURE);
          }
          add_to_fds(&fds, newfd, &fd_count, &fd_size);
        }
      } else {
        int fd = events[i].data.fd;
        if (events[i].events & EPOLLIN) {
          nbytes = recv(fd, buf, sizeof buf, 0);
          if (nbytes <= 0) {
            if (nbytes < 0) {
              perror("recv");
            } else {
              printf("epoll: socket %d hung up\n", fd);
              if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
                perror("epoll_ctl EPOLL_CTL_DEL");
                exit(EXIT_FAILURE);
              }
            }
            close(fd);
            del_from_fds(&fds, fd, &fd_count);
          } else {
            for (j = 0; j < fd_count; j++) {
              int dest_fd = fds[j];
              if (dest_fd != fd && dest_fd != listener) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                  perror("send failed");
                  exit(EXIT_FAILURE);
                }
              }
            }
          }
        }
      }
    }
  }
}
