#ifndef SNAKE_CONTROLLER_HPP
#define SNAKE_CONTROLLER_HPP

#include <QObject>
#include <QTimer> 
#include <deque>
#include "../../../brick_game/snake/snake_game.hpp"

class SnakeController : public QObject
{
    Q_OBJECT

public:
    explicit SnakeController(s21::SnakeGame &model, QObject *parent = nullptr);

    void OnTick();
    void OnAccelerate();
    void OnChangeDirection(s21::Direction dir);
    void OnTogglePause();
    void RestartGame();

signals:
    void Updated();

private:
    s21::SnakeGame &model_;
    QTimer *timer_;
    bool accelerated_ = false;
};

#endif // SNAKE_CONTROLLER_HPP
