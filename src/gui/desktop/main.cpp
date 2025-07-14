#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>

#include "mainwindow.hpp"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  try {
    QStringList games = {"Snake", "Tetris"};
    bool ok = false;
    QString selectedGame =
        QInputDialog::getItem(nullptr, "Choose Game",
                              "Select a game to start:", games, 0, false, &ok);

    if (!ok || selectedGame.isEmpty()) {
      return 0;
    }

    MainWindow mainWindow;

    if (selectedGame == "Snake") {
      mainWindow.ShowSnake();
    } else if (selectedGame == "Tetris") {
      mainWindow.ShowTetris();
    }

    mainWindow.show();
    return app.exec();

  } catch (const std::exception &e) {
    QMessageBox::critical(nullptr, "Fatal Error", e.what());
    return -1;
  } catch (...) {
    QMessageBox::critical(nullptr, "Fatal Error",
                          "Unknown exception occurred.");
    return -1;
  }
}
