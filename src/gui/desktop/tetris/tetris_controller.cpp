#include "tetris_controller.hpp"

TetrisController::TetrisController(s21::TetrisGame &model, QObject *parent)
    : QObject(parent), model_(model) {
  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &TetrisController::Update);
  timer_->start(16);
}

void TetrisController::Update() {
  model_.Update();
  emit Updated();
}

void TetrisController::MoveLeft() { model_.MoveLeft(); }

void TetrisController::MoveRight() { model_.MoveRight(); }

void TetrisController::MoveDown() { model_.MoveDown(); }

void TetrisController::Rotate() { model_.Rotate(); }

void TetrisController::TogglePause() { model_.TogglePause(); }

void TetrisController::StartGame() { model_.StartGame(); }

void TetrisController::TerminateGame() { model_.TerminateGame(); }

void TetrisController::RestartGame() { model_.StartGame(); }