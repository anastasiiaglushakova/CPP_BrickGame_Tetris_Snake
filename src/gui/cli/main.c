#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tetris/tetris_view_cli.h"
#include "snake/snake_view_cli.h"

typedef enum
{
  MENU_NONE,
  MENU_TETRIS,
  MENU_SNAKE,
  MENU_QUIT
} MenuChoice;

static MenuChoice ShowGameSelectionMenu(void)
{
  nodelay(stdscr, FALSE);
  clear();
  int y = LINES / 2;
  int x = COLS / 2 - 10;
  mvprintw(y - 2, x, "=== BRICK GAME ===");
  mvprintw(y, x, "[1] Tetris");
  mvprintw(y + 1, x, "[2] Snake");
  mvprintw(y + 3, x, "[Q] Quit");
  refresh();

  while (true)
  {
    int key = getch();
    switch (key)
    {
    case '1':
      nodelay(stdscr, TRUE);
      return MENU_TETRIS;
    case '2':
      nodelay(stdscr, TRUE);
      return MENU_SNAKE;
    case 'q':
    case 'Q':
      nodelay(stdscr, TRUE);
      return MENU_QUIT;
    default:
      break;
    }
  }
}

int main(void)
{
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);

  while (true)
  {
    MenuChoice choice = ShowGameSelectionMenu();
    if (choice == MENU_QUIT)
      break;

    if (choice == MENU_TETRIS)
    {
      RunTetris();
    }
    else if (choice == MENU_SNAKE)
    {

      RunSnake();
    }
  }

  endwin();
  return 0;
}