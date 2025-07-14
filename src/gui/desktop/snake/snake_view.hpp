#ifndef SNAKE_VIEW_HPP
#define SNAKE_VIEW_HPP

#include <QWidget>
#include "../../../brick_game/snake/snake_game.hpp"
#include "snake_controller.hpp"

class QKeyEvent;

class SnakeView : public QWidget {
    Q_OBJECT

public:
    explicit SnakeView(s21::SnakeGame &model, SnakeController &controller, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    s21::SnakeGame &model_;
    SnakeController &controller_;

    inline static constexpr int CELL_SIZE = 20;
};

#endif // SNAKE_VIEW_HPP
