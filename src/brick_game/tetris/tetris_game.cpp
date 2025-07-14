#include "tetris_game.hpp"

namespace s21 {

TetrisGame::TetrisGame() { state_ = updateCurrentState(); }

void TetrisGame::StartGame() {
  userInput(Start, false);
  UpdateState();
}

void TetrisGame::TogglePause() {
  userInput(Pause, false);
  UpdateState();
}

void TetrisGame::TerminateGame() {
  userInput(Terminate, false);
  UpdateState();
}

void TetrisGame::MoveLeft() {
  userInput(Left, false);
  UpdateState();
}

void TetrisGame::MoveRight() {
  userInput(Right, false);
  UpdateState();
}

void TetrisGame::MoveDown() {
  userInput(Down, false);
  UpdateState();
}

void TetrisGame::Rotate() {
  userInput(Action, false);
  UpdateState();
}

void TetrisGame::Update() { UpdateState(); }

int TetrisGame::GetScore() const { return state_.score; }

int TetrisGame::GetHighScore() const { return state_.high_score; }

int TetrisGame::GetLevel() const { return state_.level; }

int TetrisGame::GetSpeed() const { return state_.speed; }

bool TetrisGame::IsPaused() const { return state_.pause; }

void TetrisGame::UpdateState() { state_ = updateCurrentState(); }

}  // namespace s21
