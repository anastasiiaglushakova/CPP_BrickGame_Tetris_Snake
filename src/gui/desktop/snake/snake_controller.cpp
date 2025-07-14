#include "snake_controller.hpp"

#include <QTimer>

SnakeController::SnakeController(s21::SnakeGame &model, QObject *parent)
    : QObject(parent), model_(model) {
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &SnakeController::OnTick);
  timer_->start(model_.GetTickDelay());
}

void SnakeController::OnTick() {
  model_.Tick();
  emit Updated();

  int delay = model_.GetTickDelay();
  if (timer_->interval() != delay) {
    timer_->setInterval(delay);
  }
}

void SnakeController::OnAccelerate() { model_.Accelerate(); }

void SnakeController::OnChangeDirection(s21::Direction dir) {
  model_.ChangeDirection(dir);
}

void SnakeController::OnTogglePause() {
  if (model_.GetState() == s21::GameFSM::State::RUNNING)
    model_.Pause();
  else if (model_.GetState() == s21::GameFSM::State::PAUSED)
    model_.Resume();
}

void SnakeController::RestartGame() {
  model_.Restart();
  timer_->setInterval(model_.GetTickDelay());
}
