#include "mainwindow.hpp"

#include <QApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      stackedWidget_(nullptr),
      snakeController_(nullptr),
      tetrisController_(nullptr),
      snakeView_(nullptr),
      tetrisView_(nullptr),
      gameMenu_(nullptr),
      actionSnake_(nullptr),
      actionTetris_(nullptr),
      actionQuit_(nullptr) {
  SetupUI();
  SetupMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::SetupUI() {
  stackedWidget_ = new QStackedWidget(this);
  setCentralWidget(stackedWidget_);

  snakeController_ = new SnakeController(snakeModel_, this);
  snakeView_ = new SnakeView(snakeModel_, *snakeController_, this);
  stackedWidget_->addWidget(snakeView_);

  tetrisController_ = new TetrisController(tetrisModel_, this);
  tetrisView_ = new TetrisView(tetrisModel_, *tetrisController_, this);
  stackedWidget_->addWidget(tetrisView_);
}

void MainWindow::SetupMenu() {
  gameMenu_ = menuBar()->addMenu(tr("&Game"));

  actionSnake_ = new QAction(tr("&Snake"), this);
  actionTetris_ = new QAction(tr("&Tetris"), this);
  actionQuit_ = new QAction(tr("&Quit"), this);

  gameMenu_->addAction(actionSnake_);
  gameMenu_->addAction(actionTetris_);
  gameMenu_->addSeparator();
  gameMenu_->addAction(actionQuit_);

  connect(actionSnake_, &QAction::triggered, this, &MainWindow::OnShowSnake);
  connect(actionTetris_, &QAction::triggered, this, &MainWindow::OnShowTetris);
  connect(actionQuit_, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::OnShowSnake() {
  stackedWidget_->setCurrentWidget(snakeView_);
  snakeController_->RestartGame();
  tetrisController_->TogglePause();
}

void MainWindow::OnShowTetris() {
  stackedWidget_->setCurrentWidget(tetrisView_);
  tetrisController_->RestartGame();
  snakeController_->OnTogglePause();
}

void MainWindow::ShowSnake() { OnShowSnake(); }

void MainWindow::ShowTetris() { OnShowTetris(); }