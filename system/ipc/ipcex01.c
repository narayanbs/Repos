#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigint_handler(int sig) {
  /* using a char[] so that sizeof will work */
  const char msg[] = "Ahhh! SIGINT!\n";
  write(0, msg, sizeof(msg));
}

int main(void) {
  char s[200];

  struct sigaction sa = {
      .sa_handler = sigint_handler,
      .sa_flags = 0,  // or SA_RESTART for restarting.
      .sa_mask = 0,   // which signals to block when sa_handler is running
  };

  /*
   * Note: By default current signal is blocked during its own handler.
   * i.e:  when a handler is running for SIGUSR1, SIGUSR1 is blocked.
   * We can mention which signals to additionally block when running the handler as shown below
   *
   * Here we are blocking SIGINT
   *
   * sigemptyset(&sa.sa_mask);
   * sigaddset(&sa.sa_mask, SIGINT);
   *
   */

  /*
   * Register handlers for signals
   * You can register seperate handlers or use the same one for
   * different signals
   * for ex:
   *
   * sigaction(SIGINT, &sa_int, NULL)
   * sigactoin(SIGTERM, &sa_term, NULL)
   *  or
   * sigaction(SIGINT,  &sa, NULL);
   * sigaction(SIGTERM, &sa, NULL);
   * sigaction(SIGHUP,  &sa, NULL);
   */

  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("Enter a string:\n");

  if (fgets(s, sizeof s, stdin) == NULL)
    perror("fgets");
  else
    printf("You entered: %s\n", s);

  return 0;
}
