#include "snake_game.hpp"

#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <random>

std::filesystem::path GetExecutableDir() {
  char buffer[1024];
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (len == -1) return std::filesystem::current_path();  // fallback
  buffer[len] = '\0';
  std::filesystem::path exe_path(buffer);
  return exe_path.parent_path();
}

namespace s21 {

SnakeGame::SnakeGame()
    : dir_(Direction::UP),
      next_dir_(Direction::UP),
      has_next_dir_(false),
      width_(10),
      height_(20),
      score_(0),
      high_score_(0),
      level_(1),
      tick_delay_(300),
      accelerated_(false) {
  snake_ = {{5, 10}, {5, 11}, {5, 12}, {5, 13}};
  fsm_.SetState(GameFSM::State::RUNNING);
  high_score_ = LoadHighScore();
  SpawnApple();
}

void SnakeGame::Tick() {
  if (fsm_.GetState() != GameFSM::State::RUNNING) return;
  Move();
  accelerated_ = false;
}

void SnakeGame::Accelerate() { accelerated_ = true; }

void SnakeGame::Move() {
  if (has_next_dir_) {
    dir_ = next_dir_;
    has_next_dir_ = false;
  }

  int steps = accelerated_ ? 2 : 1;
  for (int i = 0; i < steps; ++i) {
    Point head = snake_.front();

    switch (dir_) {
      case Direction::UP:
        head.y -= 1;
        break;
      case Direction::DOWN:
        head.y += 1;
        break;
      case Direction::LEFT:
        head.x -= 1;
        break;
      case Direction::RIGHT:
        head.x += 1;
        break;
    }

    if (head.x < 0 || head.y < 0 || head.x >= width_ || head.y >= height_ ||
        CollidesWithSelf(head)) {
      fsm_.SetState(GameFSM::State::LOSE);
      return;
    }

    snake_.push_front(head);

    if (head == apple_.GetPosition()) {
      ++score_;
      if (score_ > high_score_) {
        high_score_ = score_;
        SaveHighScore(high_score_);
      }

      level_ = std::min(score_ / 5 + 1, 10);
      tick_delay_ = 300 - (level_ - 1) * 20;
      if (tick_delay_ < 100) tick_delay_ = 100;

      if (snake_.size() >= 200) {
        fsm_.SetState(GameFSM::State::WIN);
        return;
      }
      SpawnApple();
    } else {
      snake_.pop_back();
    }
  }
}

bool SnakeGame::Collides(const Point &pt) const {
  for (const auto &segment : snake_) {
    if (segment == pt) return true;
  }
  return false;
}

bool SnakeGame::CollidesWithSelf(const Point &head) const {
  for (size_t i = 1; i < snake_.size(); ++i) {
    if (snake_[i] == head) return true;
  }
  return false;
}

void SnakeGame::SpawnApple() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dx(0, width_ - 1);
  std::uniform_int_distribution<> dy(0, height_ - 1);
  Point new_apple;
  do {
    new_apple = {dx(gen), dy(gen)};
  } while (Collides(new_apple));
  apple_.SetPosition(new_apple);
}

const std::deque<Point> &SnakeGame::GetSnake() const { return snake_; }

Point SnakeGame::GetApple() const { return apple_.GetPosition(); }

GameFSM::State SnakeGame::GetState() const { return fsm_.GetState(); }

int SnakeGame::GetScore() const { return score_; }

int SnakeGame::GetHighScore() const { return high_score_; }

int SnakeGame::GetLevel() const { return level_; }

int SnakeGame::GetTickDelay() const { return tick_delay_; }

int SnakeGame::LoadHighScore() {
  auto file_path = GetExecutableDir() / "snake_highscore.txt";
  std::ifstream file(file_path);
  int score = 0;
  if (file >> score) return score;
  return 0;
}

void SnakeGame::SaveHighScore(int score) {
  auto file_path = GetExecutableDir() / "snake_highscore.txt";
  std::ofstream file(file_path);
  if (file) file << score;
}

void SnakeGame::SetApple(const Point &pos) { apple_.SetPosition(pos); }

void SnakeGame::SetSnake(const std::deque<Point> &s) { snake_ = s; }

void SnakeGame::SetState(GameFSM::State s) { fsm_.SetState(s); }

void SnakeGame::Pause() {
  if (fsm_.GetState() == GameFSM::State::RUNNING)
    fsm_.SetState(GameFSM::State::PAUSED);
}

void SnakeGame::Resume() {
  if (fsm_.GetState() == GameFSM::State::PAUSED)
    fsm_.SetState(GameFSM::State::RUNNING);
}

void SnakeGame::ChangeDirection(Direction d) {
  if ((dir_ == Direction::UP && d == Direction::DOWN) ||
      (dir_ == Direction::DOWN && d == Direction::UP) ||
      (dir_ == Direction::LEFT && d == Direction::RIGHT) ||
      (dir_ == Direction::RIGHT && d == Direction::LEFT))
    return;
  next_dir_ = d;
  has_next_dir_ = true;
}

void SnakeGame::Restart() {
  snake_ = {{5, 10}, {5, 11}, {5, 12}, {5, 13}};
  score_ = 0;
  dir_ = Direction::UP;
  next_dir_ = Direction::UP;
  has_next_dir_ = false;
  accelerated_ = false;
  level_ = 1;
  tick_delay_ = 300;
  fsm_.SetState(GameFSM::State::RUNNING);
  SpawnApple();
}

}  // namespace s21