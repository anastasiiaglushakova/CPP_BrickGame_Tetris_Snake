#include "snake_view.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>

SnakeView::SnakeView(s21::SnakeGame &model, SnakeController &controller,
                     QWidget *parent)
    : QWidget(parent), model_(model), controller_(controller) {
  setFixedSize(model_.GetWidth() * CELL_SIZE + 160,
               model_.GetHeight() * CELL_SIZE);
  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  connect(&controller_, &SnakeController::Updated, this,
          QOverload<>::of(&QWidget::update));
}

void SnakeView::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::black);

  int fieldWidth = model_.GetWidth();
  int fieldHeight = model_.GetHeight();
  int gameAreaWidth = fieldWidth * CELL_SIZE;
  int totalWidth = gameAreaWidth + 160;

  QRect gameRect(0, 0, gameAreaWidth, fieldHeight * CELL_SIZE);
  painter.setBrush(Qt::black);
  painter.drawRect(gameRect);

  painter.setBrush(Qt::red);
  auto apple = model_.GetApple();
  painter.drawRect(apple.x * CELL_SIZE, apple.y * CELL_SIZE, CELL_SIZE,
                   CELL_SIZE);

  painter.setBrush(Qt::green);
  for (const auto &segment : model_.GetSnake()) {
    painter.drawRect(segment.x * CELL_SIZE, segment.y * CELL_SIZE, CELL_SIZE,
                     CELL_SIZE);
  }

  int infoX = gameAreaWidth + 10;
  int blockWidth = totalWidth - gameAreaWidth - 20;

  QFont fontNormal("Arial", 12);
  QFont fontBold("Arial", 14, QFont::Bold);

  auto drawCenteredText = [&](int y, const QString &text, const QFont &font) {
    painter.setFont(font);
    int textWidth = QFontMetrics(font).horizontalAdvance(text);
    int x = infoX + (blockWidth - textWidth) / 2;
    painter.drawText(x, y, text);
  };

  painter.setPen(Qt::white);
  drawCenteredText(30, QString("Score: %1").arg(model_.GetScore()), fontBold);
  drawCenteredText(55, QString("High Score: %1").arg(model_.GetHighScore()),
                   fontBold);
  drawCenteredText(80, QString("Level: %1").arg(model_.GetLevel()), fontBold);
  drawCenteredText(105, QString("Speed: %1 ms").arg(model_.GetTickDelay()),
                   fontBold);

  drawCenteredText(135, "Controls:", fontBold);
  drawCenteredText(155, "← ↑ ↓ → : Move", fontNormal);
  drawCenteredText(175, "Space    : Accelerate", fontNormal);
  drawCenteredText(195, "P        : Pause", fontNormal);
  drawCenteredText(215, "Enter    : Restart", fontNormal);
  drawCenteredText(235, "Q        : Quit", fontNormal);

  QString stateText;
  switch (model_.GetState()) {
    case s21::GameFSM::State::PAUSED:
      stateText = "PAUSED";
      break;
    case s21::GameFSM::State::WIN:
      stateText = "YOU WIN!";
      break;
    case s21::GameFSM::State::LOSE:
      stateText = "GAME OVER";
      break;
    default:
      break;
  }

  if (!stateText.isEmpty()) {
    painter.setPen(Qt::yellow);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(gameRect, Qt::AlignCenter, stateText);
  }
}

void SnakeView::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Up:
      controller_.OnChangeDirection(s21::Direction::UP);
      break;

    case Qt::Key_Down:
      controller_.OnChangeDirection(s21::Direction::DOWN);
      break;

    case Qt::Key_Left:
      controller_.OnChangeDirection(s21::Direction::LEFT);
      break;

    case Qt::Key_Right:
      controller_.OnChangeDirection(s21::Direction::RIGHT);
      break;

    case Qt::Key_Space:
      controller_.OnAccelerate();
      break;

    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (model_.GetState() != s21::GameFSM::State::RUNNING)
        controller_.RestartGame();
      break;

    case Qt::Key_P:
    case Qt::Key_P + 32:  // p
      controller_.OnTogglePause();
      break;

    case Qt::Key_Q:
    case Qt::Key_Q + 32:  // q
      qApp->quit();
      break;

    default:
      QWidget::keyPressEvent(event);
  }
}
