#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define TB_HEIGHT 5
#define TB_WIDTH 30
#define COUNT_DROPS 100
#define SLEEP_DELAY 100000 // useconds

typedef struct Raindrop {
  float x, y, vx, vy;
} Raindrop;

Raindrop drops[COUNT_DROPS];

float vx;

/* Generate a new raindrop at index i in the global raindrops array
 * If scatter is set true, then raindrops are scattered on the window, otherwise
 * their coordinate is set 0 */
void genRaindrop(int i, bool scatter) {
  int width, height;
  getmaxyx(stdscr, height, width);

  drops[i].x = rand() % (width - 2); // number between 0 and width
  drops[i].y =
      scatter ? rand() % (height - 2) : 0; // number between 0 and height
  drops[i].vx = vx;
  drops[i].vy = 1;
}

void moveRain(WINDOW *rain_win) {
  int width, height;
  getmaxyx(rain_win, height, width);

  for (int i = 0; i < COUNT_DROPS; i++) {
    drops[i].x += drops[i].vx;
    drops[i].y += drops[i].vy;

    // has raindrop left window?
    if (drops[i].x > width || drops[i].y > height) {
      genRaindrop(i, false);
    }
  }
}

WINDOW *drawRain() {
  int width, height;
  getmaxyx(stdscr, height, width);

  WINDOW *rain_win = newwin(height - 2, width - 2, 1, 1);

  char rain_chars[] = {'.', '`', '\'', ',', '*'};

  // box(rain_win, 0, 0);

  for (int i = 0; i < COUNT_DROPS; i++) {
    int char_i =
        abs(drops[i].vx) % (sizeof(rain_chars) / sizeof(rain_chars[0]));

    float wind = sin(time(NULL) * 0.1) * 2;
    drops[i].x += drops[i].vx + wind;

    mvwaddch(rain_win, drops[i].y, drops[i].x, rain_chars[char_i]);
  }

  // wrefresh(rain_win);
  // delwin(rain_win);
  return rain_win;
}

void initRaindrops() {
  vx = rand() % 9 - 4; // number between -4 and 4
  for (int i = 0; i < COUNT_DROPS; i++) {
    genRaindrop(i, true);
  }
}

WINDOW *drawTitleBox() {
  int width, height;
  getmaxyx(stdscr, height, width);

  WINDOW *title_win = newwin(TB_HEIGHT, TB_WIDTH, (height - TB_HEIGHT) / 2,
                             (width - TB_WIDTH) / 2);
  box(title_win, 0, 0);

  char *username = getlogin();

  if (username == NULL) {
    username = getenv("USER");
  }

  char buffer[100];
  snprintf(buffer, sizeof(buffer), "Hello %s!", username);

  mvwprintw(title_win, (TB_HEIGHT / 2), (TB_WIDTH - strlen(buffer)) / 2, buffer,
            username);

  // wrefresh(title_win);
  return title_win;
}

void drawOuterBoundaryBox() { box(stdscr, 0, 0); }

int main() {
  srand(time(NULL));
  initscr();
  initRaindrops();

  int width, height;
  getmaxyx(stdscr, height, width);

  WINDOW *rain_win, *title_win;

  // start_color();
  // int_pair(1, COLOR_CYAN, COLOR_BLACK);
  // int_pair(2, COLOR_WHITE, COLOR_BLACK);
  // wattron(rain_win, COLOR_PAIR(1));

  drawOuterBoundaryBox();

  char *wh_msg = "Width=%d, Height=%d";

  char buffer[100];
  snprintf(buffer, sizeof(buffer), wh_msg, width, height);
  mvprintw(height - 1, width / 2 - strlen(buffer) / 2, wh_msg, width, height);

  refresh();

  // int t = 0;
  while (true) {
    // int x, y;
    // x = 5;
    // y = 5;
    // mvprintw(x, y, "Hello %s, Press Enter To Exit!", username);

    rain_win = drawRain();

    title_win = drawTitleBox();

    moveRain(rain_win);

    wnoutrefresh(rain_win);
    wnoutrefresh(title_win);
    doupdate();

    delwin(rain_win);
    delwin(title_win);

    usleep(SLEEP_DELAY);
  }

  getch();
  endwin();

  return 0;
}
