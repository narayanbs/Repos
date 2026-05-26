#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * for handling multiple signals using the same handler
 *
 *  if (sig == SIGINT)
 *     printf("SIGINT received\n");
 *  else if (sig == SIGTERM)
 *     printf("SIGTERM received\n");
 */
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
      .sa_mask = 0,
  };

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
