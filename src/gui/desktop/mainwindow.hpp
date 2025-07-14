#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "../../brick_game/snake/snake_game.hpp"
#include "../../brick_game/tetris/tetris_game.hpp"

#include "snake/snake_controller.hpp"
#include "snake/snake_view.hpp"
#include "tetris/tetris_controller.hpp"
#include "tetris/tetris_view.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void ShowSnake();
    void ShowTetris();

private slots:
    void OnShowSnake();
    void OnShowTetris();

private:
    void SetupUI();
    void SetupMenu();

    QStackedWidget *stackedWidget_;

    s21::SnakeGame snakeModel_;
    s21::TetrisGame tetrisModel_;

    SnakeController *snakeController_;
    TetrisController *tetrisController_;

    SnakeView *snakeView_;
    TetrisView *tetrisView_;

    QMenu *gameMenu_;
    QAction *actionSnake_;
    QAction *actionTetris_;
    QAction *actionQuit_;
};

#endif // MAINWINDOW_HPP