#ifndef SNAKE_VIEW_CLI_H
#define SNAKE_VIEW_CLI_H

#include <ncurses.h>
#include <stdbool.h>

#include "../../../brick_game/snake/snake_c_api.h"

void SnakeCLI_Initialize(void *game_ptr);

void SnakeCLI_ShowMainMenu(void);

void SnakeCLI_DrawGame(void *game_ptr);

void SnakeCLI_HandleKey(int key, void *game_ptr);

bool SnakeCLI_IsRunning(void);

void RunSnake(void);

#endif  // SNAKE_VIEW_CLI_H
