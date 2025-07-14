#include "snake_c_api.h"

#include <algorithm>

#include "snake_game.hpp"

using namespace s21;

extern "C" {

struct SnakeWrapper {
  SnakeGame game;
};

void *snake_create() { return new SnakeWrapper(); }

void snake_destroy(void *game_ptr) {
  if (game_ptr) {
    delete static_cast<SnakeWrapper *>(game_ptr);
  }
}

void snake_tick(void *game_ptr) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.Tick();
  }
}

void snake_change_direction(void *game_ptr, SnakeDirection dir) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.ChangeDirection(
        static_cast<s21::Direction>(dir));
  }
}

void snake_accelerate(void *game_ptr) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.Accelerate();
  }
}

void snake_pause(void *game_ptr) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.Pause();
  }
}

void snake_resume(void *game_ptr) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.Resume();
  }
}

void snake_restart(void *game_ptr) {
  if (game_ptr) {
    static_cast<SnakeWrapper *>(game_ptr)->game.Restart();
  }
}

SnakeState snake_get_state(void *game_ptr) {
  if (!game_ptr) return SNAKE_STATE_INIT;
  auto state = static_cast<SnakeWrapper *>(game_ptr)->game.GetState();
  switch (state) {
    case GameFSM::State::INIT:
      return SNAKE_STATE_INIT;
    case GameFSM::State::RUNNING:
      return SNAKE_STATE_RUNNING;
    case GameFSM::State::PAUSED:
      return SNAKE_STATE_PAUSED;
    case GameFSM::State::WIN:
      return SNAKE_STATE_WIN;
    case GameFSM::State::LOSE:
      return SNAKE_STATE_LOSE;
    default:
      return SNAKE_STATE_INIT;
  }
}

int snake_get_score(void *game_ptr) {
  if (!game_ptr) return 0;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetScore();
}

int snake_get_level(void *game_ptr) {
  if (!game_ptr) return 1;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetLevel();
}

int snake_get_width(void *game_ptr) {
  if (!game_ptr) return 0;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetWidth();
}

int snake_get_height(void *game_ptr) {
  if (!game_ptr) return 0;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetHeight();
}

int snake_get_snake_points(void *game_ptr, SnakePoint *snake_array,
                           int max_points) {
  if (!game_ptr || !snake_array || max_points <= 0) return 0;
  const auto &snake = static_cast<SnakeWrapper *>(game_ptr)->game.GetSnake();
  int count = std::min(static_cast<int>(snake.size()), max_points);
  for (int i = 0; i < count; ++i) {
    snake_array[i].x = snake[i].x;
    snake_array[i].y = snake[i].y;
  }
  return count;
}

SnakePoint snake_get_apple_point(void *game_ptr) {
  SnakePoint pt = {0, 0};
  if (!game_ptr) return pt;
  Point apple = static_cast<SnakeWrapper *>(game_ptr)->game.GetApple();
  pt.x = apple.x;
  pt.y = apple.y;
  return pt;
}

int snake_get_high_score(void *game_ptr) {
  if (!game_ptr) return 0;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetHighScore();
}

int snake_get_tick_delay(void *game_ptr) {
  if (!game_ptr) return 0;
  return static_cast<SnakeWrapper *>(game_ptr)->game.GetTickDelay();
}

}  // extern "C"
