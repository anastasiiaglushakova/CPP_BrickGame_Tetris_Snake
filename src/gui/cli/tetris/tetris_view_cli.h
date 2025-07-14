#ifndef TETRIS_VIEW_CLI_H
#define TETRIS_VIEW_CLI_H

#include <ncurses.h>

#include "../../../brick_game/tetris/tetris_c_api.h"

void InitializeInterface(void);
void ShowMainMenu(void);
void DrawGame(GameInfo_t state);
UserAction_t GetActionFromKey(int key);
bool IsGameRunning(void);
void RunTetris(void);

#endif  // TETRIS_VIEW_CLI_H
