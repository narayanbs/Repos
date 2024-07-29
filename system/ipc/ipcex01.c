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
      .sa_flags = 0, // or SA_RESTART for restarting.
      .sa_mask = 0,
  };

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
