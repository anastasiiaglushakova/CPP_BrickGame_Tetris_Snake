#include "tetris_view.hpp"

#include <QApplication>
#include <QFont>
#include <QPainter>

TetrisView::TetrisView(s21::TetrisGame &model, TetrisController &controller,
                       QWidget *parent)
    : QWidget(parent), model_(model), controller_(controller) {
  setFixedSize(model_.GetWidth() * CELL_SIZE + 150,
               model_.GetHeight() * CELL_SIZE);
  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  connect(&controller_, &TetrisController::Updated, this,
          QOverload<>::of(&QWidget::update));
}

void TetrisView::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::black);

  int **field = model_.GetField();
  int width = model_.GetWidth();
  int height = model_.GetHeight();

  auto getColorForCell = [](int cell) -> QColor {
    switch (cell) {
      case 1:
        return Qt::red;
      case 2:
        return Qt::green;
      case 3:
        return Qt::blue;
      case 4:
        return Qt::magenta;
      case 5:
        return Qt::yellow;
      case 6:
        return Qt::white;
      case 7:
        return Qt::cyan;
      default:
        return Qt::gray;
    }
  };

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int cell = field[y][x];
      if (cell != 0) {
        QColor color = getColorForCell(cell);
        painter.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                         color);
        painter.setPen(Qt::black);
        painter.drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
      }
    }
  }

  if (model_.IsPaused()) {
    painter.setPen(Qt::yellow);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "PAUSED");
  }

  int borderX = model_.GetWidth() * CELL_SIZE;
  int borderHeight = model_.GetHeight() * CELL_SIZE;
  painter.setPen(Qt::white);
  painter.drawLine(borderX, 0, borderX, borderHeight);

  int offsetX = model_.GetWidth() * CELL_SIZE + 10;
  int rightBlockWidth = 160;

  QFont boldFont("Arial", 14, QFont::Bold);
  QFont normalFont("Arial", 12);

  QFontMetrics fmBold(boldFont);
  QFontMetrics fmNormal(normalFont);

  int nextOffsetY = 30;
  painter.setFont(boldFont);
  painter.setPen(Qt::white);

  QString nextText = "Next:";
  int nextTextWidth = fmBold.horizontalAdvance(nextText);
  int nextTextX = offsetX + (rightBlockWidth - nextTextWidth) / 2;
  painter.drawText(nextTextX, nextOffsetY, nextText);

  const int nextCellSize = CELL_SIZE;
  nextOffsetY += 10;

  int **nextMatrix = model_.GetNextMatrix();
  if (nextMatrix) {
    for (int i = 0; i < FIGURE_SIZE; ++i) {
      for (int j = 0; j < FIGURE_SIZE; ++j) {
        int cell = nextMatrix[i][j];
        if (cell != 0) {
          QColor color = getColorForCell(cell);
          int cellX = offsetX +
                      (rightBlockWidth - FIGURE_SIZE * nextCellSize) / 2 +
                      j * nextCellSize;
          int cellY = nextOffsetY + i * nextCellSize;
          painter.fillRect(cellX, cellY, nextCellSize, nextCellSize, color);
          painter.setPen(Qt::black);
          painter.drawRect(cellX, cellY, nextCellSize, nextCellSize);
          painter.setPen(Qt::white);
        }
      }
    }
  }

  int infoOffsetY = nextOffsetY + FIGURE_SIZE * nextCellSize + 20;
  painter.setFont(normalFont);

  auto drawCenteredText = [&](int y, const QString &text) {
    int textWidth = fmNormal.horizontalAdvance(text);
    int textX = offsetX + (rightBlockWidth - textWidth) / 2;
    painter.drawText(textX, y, text);
  };

  drawCenteredText(infoOffsetY, QString("Score: %1").arg(model_.GetScore()));
  drawCenteredText(infoOffsetY + 20,
                   QString("High Score: %1").arg(model_.GetHighScore()));
  drawCenteredText(infoOffsetY + 40,
                   QString("Level: %1").arg(model_.GetLevel()));
  drawCenteredText(infoOffsetY + 60,
                   QString("Speed: %1 ms").arg(model_.GetSpeed()));

  drawCenteredText(infoOffsetY + 80, "Controls:");
  drawCenteredText(infoOffsetY + 100, "← → : Move");
  drawCenteredText(infoOffsetY + 120, "↑    : Rotate");
  drawCenteredText(infoOffsetY + 140, "↓    : Drop");
  drawCenteredText(infoOffsetY + 160, "P    : Pause");
  drawCenteredText(infoOffsetY + 180, "Enter: Start");
  drawCenteredText(infoOffsetY + 200, "Q    : Quit");
}

void TetrisView::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Left:
      controller_.MoveLeft();
      break;
    case Qt::Key_Right:
      controller_.MoveRight();
      break;
    case Qt::Key_Down:
      controller_.MoveDown();
      break;
    case Qt::Key_Up:
      controller_.Rotate();
      break;
    case Qt::Key_P:
    case Qt::Key_P + 32:
      controller_.TogglePause();
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      controller_.StartGame();
      break;
    case Qt::Key_Q:
    case Qt::Key_Q + 32:
      qApp->quit();
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void TetrisView::OnTick() { update(); }
