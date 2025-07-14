#ifndef TETRIS_VIEW_HPP
#define TETRIS_VIEW_HPP

#include <QWidget>
#include <QKeyEvent>
#include <QApplication>
#include "../../../brick_game/tetris/tetris_game.hpp"
#include "tetris_controller.hpp"

class TetrisView : public QWidget {
    Q_OBJECT

public:
    explicit TetrisView(s21::TetrisGame &model, TetrisController &controller, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void OnTick();

private:
    s21::TetrisGame &model_;
    TetrisController &controller_;

    static constexpr int CELL_SIZE = 20;
};

#endif  // TETRIS_VIEW_HPP
