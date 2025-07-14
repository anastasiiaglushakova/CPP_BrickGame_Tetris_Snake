#ifndef SNAKE_GAME_HPP
#define SNAKE_GAME_HPP

#include <deque>
#include <random>
#include <string>

namespace s21 {

enum class Direction {
  UP,
  RIGHT,
  DOWN,
  LEFT
};

struct Point {
  int x;
  int y;
  bool operator==(const Point &other) const { return x == other.x && y == other.y; }
};

class Apple {
public:
  Apple() = default;
  explicit Apple(Point pos) : position_(pos) {}
  const Point &GetPosition() const { return position_; }
  void SetPosition(const Point &pos) { position_ = pos; }

private:
  Point position_;
};

class GameFSM {
public:
  enum class State {
    INIT,
    RUNNING,
    PAUSED,
    WIN,
    LOSE
  };
  void SetState(State new_state) { state_ = new_state; }
  State GetState() const { return state_; }

private:
  State state_ = State::INIT;
};

class SnakeGame {
public:
  SnakeGame();
  void Tick();
  void Accelerate();

  const std::deque<Point> &GetSnake() const;
  Point GetApple() const;
  GameFSM::State GetState() const;
  int GetWidth() const { return width_; }
  int GetHeight() const { return height_; }
  int GetScore() const;
  int GetHighScore() const;
  int GetLevel() const;
  int GetTickDelay() const;

  void SetApple(const Point &pos);
  void SetSnake(const std::deque<Point> &s);
  void SetState(GameFSM::State s);
  void Pause();
  void Resume();
  void ChangeDirection(Direction d);
  void Restart();

private:
  void Move();
  void SpawnApple();
  bool Collides(const Point &) const;
  bool CollidesWithSelf(const Point &head) const;

  int LoadHighScore();
  void SaveHighScore(int score);

  GameFSM fsm_;
  std::deque<Point> snake_;
  Direction dir_;
  Direction next_dir_;
  bool has_next_dir_ = false;

  Apple apple_;
  int width_, height_;
  int score_;
  int high_score_;
  int level_;
  int tick_delay_;
  bool accelerated_;
};

}  // namespace s21

#endif  // SNAKE_GAME_HPP
