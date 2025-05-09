#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

const char GAMEDIR[] = "/home/root/myuart/ncurses-games/";

void nc_setup();
int popup(const char*);

const char *aop_levels[] = {
  NULL,
  "aop-level-01.txt",
  "aop-level-02.txt",
  "aop-level-03.txt",
  "aop-level-04.txt",
  "aop-level-05.txt",
  "aop-level-06.txt",
  "aop-level-07.txt",
  "aop-level-08.txt",
  "aop-level-09.txt",
  "aop-level-10.txt",
  "aop-level-11.txt"
};

const char *choices[] = {
  "Minesweeper",
  "nPush",
  "Battleships",
  "Greed",
  "Pong",
  "Ambassador of Pain",
  "Galaxis",
  "Mogria's Snake",
  "Tic Tac 4"
};

int main() {
  nc_setup();
  if (chdir(GAMEDIR) != 0) {
    perror("chdir failed for gamedir");
    return 1;
  }

  int highlight = 0;
  int choice = -1;
  int c;
  int n_choices = ARRAY_SIZE(choices);
  int height = 5;
  int width = 40;
  int starty = (LINES - height) / 2;
  int startx = (COLS - width) / 2;
  pid_t pid;

  while (1) {
    mvprintw(0, 0, "Press <q> to exit");
    attron(A_BOLD);
    mvprintw(starty - 3, startx + ((width - strlen("GAME SELECTION")) / 2), "GAME SELECTION");
    attroff(A_BOLD);

    for (int i = 0; i < n_choices; ++i) {
      if (i == highlight) {
        attron(A_REVERSE);
      }
      mvprintw(starty + i, startx + ((width - strlen(choices[i])) / 2), "%s", choices[i]);
      attroff(A_REVERSE);
    }

    c = getch();

    if (c == 'q') {
      break;
    }

    switch (c) {
      case KEY_UP:
        if (highlight > 0) {
          highlight--;
        }
        break;
      case KEY_DOWN:
        if (highlight < n_choices - 1) {
          highlight++;
        }
        break;
      case 10: // Enter key
        choice = highlight;
        break;
    }

    int value;

    switch (choice) {
      case 0:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./ncurses-minesweeper/bin/minesweeper", "./ncurses-minesweeper/bin/minesweeper", NULL);
          perror("execl failed for minesweeper");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 1:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          if (chdir("./npush-0.7/") != 0) {
            perror("chdir failed for npush");
            return 1;
          }
          execl("./npush", "./npush", NULL);
          perror("execl failed for npush");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 2:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./bs-master/bs", "./bs-master/bs", NULL);
          perror("execl failed for bs");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 3:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./greed-4.3/greed", "./greed-4.3/greed", NULL);
          perror("execl failed for greed");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 4:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./pong-0.1.0/pong", "./pong-0.1.0/pong", NULL);
          perror("execl failed for pong");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 5:
        c = popup("Enter number to select level: 1-9");
        value = c - '0';
        if (value < 1 || value > 11) {
          clear();
          break;
        }
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          if (chdir("./aop-0.6/") != 0) {
            perror("chdir failed for aop");
            return 1;
          }
          execl("./aop", "./aop", aop_levels[value], NULL);
          perror("execl failed for aop");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 6:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./galaxis-1.11/galaxis", "./galaxis-1.11/galaxis", NULL);
          perror("execl failed for galaxis");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 7:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./msnake/src/snake", "./msnake/src/snake", NULL);
          perror("execl failed for msnake");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
      case 8:
        endwin();
        pid = fork();
        if (pid == 0) { // child process
          execl("./tictac4-1.0/tictac4", "./tictac4-1.0/tictac4", NULL);
          perror("execl failed for tictac4");
          _exit(EXIT_FAILURE);
        } else if (pid > 0) { // parent process
          int status;
          waitpid(pid, &status, 0);
          nc_setup();
          clear();
          refresh();
        }
        break;
    }
    choice = -1;
  }

  endwin(); // end ncurses mode
  return 0;
}

void nc_setup() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  mvprintw(2, 0, "TERM is: %s", getenv("TERM"));
  int ret = curs_set(0);
  if (ret == ERR) {
    mvprintw(1, 0, "Warning: curs_set(0) failed.");
  }
}

int popup(const char *message) {
  int width = strlen(message) + 4;
  int height = 5;
  int starty = (LINES - height) / 2;
  int startx = (COLS - width) / 2;
  int c;

  WINDOW *popup_win = newwin(height, width, starty, startx);
  box(popup_win, 0, 0);

  mvwprintw(popup_win, 2, 2, "%s", message);
  wrefresh(popup_win);

  c = wgetch(popup_win);  // wait for user input
  delwin(popup_win);
  return c;
}
