#ifndef TETRIS_CONTROLLER_HPP
#define TETRIS_CONTROLLER_HPP

#include <QObject>
#include <QTimer>
#include "../../../brick_game/tetris/tetris_game.hpp"

class TetrisController : public QObject
{
    Q_OBJECT

public:
    explicit TetrisController(s21::TetrisGame &model, QObject *parent = nullptr);

public slots:
    void Update();
    void MoveLeft();
    void MoveRight();
    void MoveDown();
    void Rotate();
    void TogglePause();
    void StartGame();
    void TerminateGame();
    void RestartGame();

signals:
    void Updated();

private:
    s21::TetrisGame &model_;
    QTimer *timer_;
};

#endif // TETRIS_CONTROLLER_HPP
