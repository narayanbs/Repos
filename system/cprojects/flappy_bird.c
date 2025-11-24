#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DELAY 80000
#define GRAVITY 1
#define JUMP_HEIGHT 3
#define PIPE_WIDTH 6
#define PIPE_GAP 6
#define PIPE_DISTANCE 25

typedef struct {
  int x, gap_y;
} Pipe;

int main() {
  int max_y, max_x;
  int bird_y, bird_x;
  int ch;
  int score = 0;
  int velocity = 0;
  int num_pipes = 3;

  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  // sets getch as non-blocking
  nodelay(stdscr, TRUE);

  getmaxyx(stdscr, max_y, max_x);

  bird_x = max_x / 4;
  bird_y = max_y / 2;

  Pipe pipes[num_pipes];
  srand(time(0));
  for (int i = 0; i < num_pipes; i++) {
    pipes[i].x = max_x + i * PIPE_DISTANCE;
    pipes[i].gap_y = rand() % (max_y - PIPE_GAP - 4) + 2;
  }

  while (1) {
    clear();

    // Draw bird
    mvprintw(bird_y, bird_x, "@");

    // Draw pipes
    for (int i = 0; i < num_pipes; i++) {
      for (int y = 0; y < max_y; y++) {
        if (y < pipes[i].gap_y || y > pipes[i].gap_y + PIPE_GAP) {
          mvprintw(y, pipes[i].x, "######");
        }
      }
    }

    // Draw score
    mvprintw(0, 0, "Score: %d", score);

    // Input
    ch = getch();
    if (ch == ' ' || ch == KEY_UP)
      velocity = -JUMP_HEIGHT;
    else if (ch == 'q')
      break;

    // Gravity effect
    bird_y += velocity;
    velocity += GRAVITY;
    if (velocity > 2) velocity = 2;  // limit fall speed

    // Move pipes
    for (int i = 0; i < num_pipes; i++) {
      pipes[i].x--;

      // Reset pipe if off-screen
      if (pipes[i].x < -PIPE_WIDTH) {
        pipes[i].x = max_x;
        pipes[i].gap_y = rand() % (max_y - PIPE_GAP - 4) + 2;
        score++;
      }

      // Collision detection
      if (bird_x >= pipes[i].x && bird_x <= pipes[i].x + PIPE_WIDTH) {
        if (bird_y < pipes[i].gap_y || bird_y > pipes[i].gap_y + PIPE_GAP) {
          mvprintw(max_y / 2, max_x / 2 - 5, "GAME OVER!");
          mvprintw(max_y / 2 + 1, max_x / 2 - 8, "Final Score: %d", score);
          nodelay(stdscr, FALSE);
          getch();
          endwin();
          return 0;
        }
      }
    }

    // Check boundaries
    if (bird_y < 0 || bird_y >= max_y) {
      mvprintw(max_y / 2, max_x / 2 - 5, "GAME OVER!");
      mvprintw(max_y / 2 + 1, max_x / 2 - 8, "Final Score: %d", score);
      nodelay(stdscr, FALSE);
      getch();
      endwin();
      return 0;
    }

    refresh();
    usleep(DELAY);
  }

  endwin();
  return 0;
}
