#ifndef SNAKE_C_API_H
#define SNAKE_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
  SNAKE_STATE_INIT,
  SNAKE_STATE_RUNNING,
  SNAKE_STATE_PAUSED,
  SNAKE_STATE_WIN,
  SNAKE_STATE_LOSE
} SnakeState;

typedef struct {
  int x;
  int y;
} SnakePoint;

typedef enum {
  SNAKE_DIR_UP = 0,
  SNAKE_DIR_RIGHT = 1,
  SNAKE_DIR_DOWN = 2,
  SNAKE_DIR_LEFT = 3
} SnakeDirection;

void *snake_create();
void snake_destroy(void *game_ptr);

void snake_tick(void *game_ptr);
void snake_change_direction(void *game_ptr, SnakeDirection dir);
void snake_accelerate(void *game_ptr);
void snake_pause(void *game_ptr);
void snake_resume(void *game_ptr);
void snake_restart(void *game_ptr);

SnakeState snake_get_state(void *game_ptr);
int snake_get_score(void *game_ptr);
int snake_get_level(void *game_ptr);
int snake_get_width(void *game_ptr);
int snake_get_height(void *game_ptr);

int snake_get_snake_points(void *game_ptr, SnakePoint *snake_array,
                           int max_points);

SnakePoint snake_get_apple_point(void *game_ptr);

int snake_get_high_score(void *game_ptr);
int snake_get_tick_delay(void *game_ptr);

#ifdef __cplusplus
}
#endif

#endif  // SNAKE_C_API_H
