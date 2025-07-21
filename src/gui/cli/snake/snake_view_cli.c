#include "snake_view_cli.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define BORDER_CHAR "##"
#define SNAKE_CHAR "[]"
#define APPLE_CHAR "@@"
#define EMPTY_CHAR "  "

static bool g_snake_running = true;

void SnakeCLI_Initialize(void *game_ptr)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    if (!game_ptr)
    {
        fprintf(stderr, "Error: game_ptr is NULL in SnakeCLI_Initialize\n");
        exit(EXIT_FAILURE);
    }

    int w = snake_get_width(game_ptr);
    int h = snake_get_height(game_ptr);
    int min_rows = h + 6;
    int min_cols = (w + 20) * 2;

    if (rows < min_rows || cols < min_cols)
    {
        endwin();
        printf("Terminal too small. Minimum required: %dx%d\n", min_cols, min_rows);
        exit(EXIT_FAILURE);
    }
}

void SnakeCLI_ShowMainMenu(void)
{
    clear();
    int cy = LINES / 2, cx = COLS / 2;
    mvprintw(cy - 2, cx - 6, "=== SNAKE ===");
    mvprintw(cy, cx - 10, "Enter : Start");
    mvprintw(cy + 1, cx - 10, "Q     : Quit");
    refresh();
}

void SnakeCLI_DrawGame(void *game_ptr)
{
    if (!game_ptr)
        return;

    SnakeState state = snake_get_state(game_ptr);
    int w = snake_get_width(game_ptr);
    int h = snake_get_height(game_ptr);

    clear();

    for (int x = -1; x <= w; x++)
    {
        mvprintw(0, (x + 1) * 2, BORDER_CHAR);
        mvprintw(h + 1, (x + 1) * 2, BORDER_CHAR);
    }
    for (int y = 0; y <= h + 1; y++)
    {
        mvprintw(y, 0, BORDER_CHAR);
        mvprintw(y, (w + 1) * 2, BORDER_CHAR);
    }

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            mvprintw(y + 1, (x + 1) * 2, EMPTY_CHAR);

    SnakePoint apple = snake_get_apple_point(game_ptr);
    mvprintw(apple.y + 1, (apple.x + 1) * 2, APPLE_CHAR);

    SnakePoint buf[400];
    int cnt = snake_get_snake_points(game_ptr, buf, 400);
    for (int i = 0; i < cnt; ++i)
    {
        mvprintw(buf[i].y + 1, (buf[i].x + 1) * 2, SNAKE_CHAR);
    }

    int infox = (w + 4) * 2;
    mvprintw(2, infox, "Score : %d", snake_get_score(game_ptr));
    mvprintw(3, infox, "Level : %d", snake_get_level(game_ptr));
    mvprintw(4, infox, "High  : %d", snake_get_high_score(game_ptr));
    mvprintw(5, infox, "Delay : %d ms", snake_get_tick_delay(game_ptr));

    mvprintw(7, infox, "Controls:");
    mvprintw(8, infox, "Left, Right, Up, Down : Move");
    mvprintw(9, infox, "Space   : Accelerate");
    mvprintw(10, infox, "P       : Pause");
    mvprintw(11, infox, "Enter   : Restart");
    mvprintw(12, infox, "Q       : Quit");

    int center_y = h / 2 + 1;

    if (state == SNAKE_STATE_PAUSED)
    {
        int text_len = 6; // "PAUSED"
        int center_x = 2 + (w * 2 - text_len) / 2;
        mvprintw(center_y - 1, center_x, "PAUSED");
        mvprintw(center_y + 1, center_x - 3, "Continue:  P");
    }
    else if (state == SNAKE_STATE_WIN)
    {
        int text_len = 8; // "YOU WIN!"
        int center_x = 2 + (w * 2 - text_len) / 2;
        mvprintw(center_y, center_x, "YOU WIN!");
    }
    else if (state == SNAKE_STATE_LOSE)
    {
        int text_len = 9; // "GAME OVER"
        int center_x = 2 + (w * 2 - text_len) / 2;
        mvprintw(center_y, center_x, "GAME OVER");
    }

    refresh();
}

void SnakeCLI_HandleKey(int key, void *game_ptr)
{
    if (!game_ptr)
        return;

    switch (key)
    {
    case '\n':
    case '\r':
    case KEY_ENTER:
        if (snake_get_state(game_ptr) == SNAKE_STATE_LOSE || snake_get_state(game_ptr) == SNAKE_STATE_WIN)
        {
            snake_restart(game_ptr);
        }
        break;
    case 'q':
    case 'Q':
        g_snake_running = false;
        break;
    case 'p':
    case 'P':
        if (snake_get_state(game_ptr) == SNAKE_STATE_PAUSED)
            snake_resume(game_ptr);
        else if (snake_get_state(game_ptr) == SNAKE_STATE_RUNNING)
            snake_pause(game_ptr);
        break;
    case 'r':
    case 'R':
        snake_resume(game_ptr);
        break;
    case KEY_UP:
        snake_change_direction(game_ptr, SNAKE_DIR_UP);
        break;
    case KEY_DOWN:
        snake_change_direction(game_ptr, SNAKE_DIR_DOWN);
        break;
    case KEY_LEFT:
        snake_change_direction(game_ptr, SNAKE_DIR_LEFT);
        break;
    case KEY_RIGHT:
        snake_change_direction(game_ptr, SNAKE_DIR_RIGHT);
        break;
    case ' ':
        snake_accelerate(game_ptr);
        break;
    default:
        break;
    }
}

bool SnakeCLI_IsRunning(void)
{
    return g_snake_running;
}

void RunSnake(void)
{
    static void *snake = NULL;

    if (!snake)
        snake = snake_create();

    g_snake_running = true;
    SnakeCLI_Initialize(snake);

    SnakeState state = snake_get_state(snake);

    if (state == SNAKE_STATE_LOSE || state == SNAKE_STATE_WIN)
    {
        snake_restart(snake);
        snake_resume(snake);
    }

    struct timespec last_tick;
    clock_gettime(CLOCK_MONOTONIC, &last_tick);

    while (SnakeCLI_IsRunning())
    {
        int key = getch();
        if (key != ERR)
        {
            SnakeCLI_HandleKey(key, snake);

            if (!SnakeCLI_IsRunning())
                break;

            SnakeCLI_DrawGame(snake);
        }

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        long elapsed_ms = (now.tv_sec - last_tick.tv_sec) * 1000 +
                          (now.tv_nsec - last_tick.tv_nsec) / 1000000;

        state = snake_get_state(snake);
        int delay = snake_get_tick_delay(snake);

        if (state == SNAKE_STATE_RUNNING && elapsed_ms >= delay)
        {
            snake_tick(snake);
            SnakeCLI_DrawGame(snake);
            clock_gettime(CLOCK_MONOTONIC, &last_tick);
        }

        usleep(1000);
    }
}
