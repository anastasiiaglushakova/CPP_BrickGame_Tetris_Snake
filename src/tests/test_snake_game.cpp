#include <gtest/gtest.h>

#include "../brick_game/snake/snake_game.hpp"

using namespace s21;

class SnakeGameTest : public ::testing::Test {
 protected:
  SnakeGame game;

  Point GetHead() const {
    const auto &snake = game.GetSnake();
    return snake.front();
  }
};

TEST_F(SnakeGameTest, ConstructorSetsInitialValues) {
  EXPECT_EQ(game.GetState(), GameFSM::State::RUNNING);
  EXPECT_EQ(game.GetScore(), 0);
  EXPECT_EQ(game.GetLevel(), 1);
  EXPECT_GE(game.GetTickDelay(), 100);
  EXPECT_LE(game.GetTickDelay(), 300);
  EXPECT_EQ(game.GetWidth(), 10);
  EXPECT_EQ(game.GetHeight(), 20);
  EXPECT_FALSE(game.GetSnake().empty());

  Point apple = game.GetApple();
  EXPECT_GE(apple.x, 0);
  EXPECT_LT(apple.x, game.GetWidth());
  EXPECT_GE(apple.y, 0);
  EXPECT_LT(apple.y, game.GetHeight());
}

TEST_F(SnakeGameTest, ChangeDirectionAllowsValidTurns) {
  game.ChangeDirection(Direction::LEFT);
  game.Tick();
  Point head = GetHead();
  EXPECT_EQ(head.x, 4);
}

TEST_F(SnakeGameTest, MoveDownDirectionCovered) {
  game.ChangeDirection(Direction::LEFT);
  game.Tick();

  game.ChangeDirection(Direction::DOWN);
  game.Tick();

  Point head = GetHead();

  EXPECT_EQ(head.x, 4);
  EXPECT_EQ(head.y, 11);
}

TEST_F(SnakeGameTest, ChangeDirectionPreventsReverse) {
  game.ChangeDirection(Direction::DOWN);
  game.Tick();
  Point head = GetHead();
  EXPECT_NE(head.y, 11);
}

TEST_F(SnakeGameTest, MoveOneStepForward) {
  Point old_head = GetHead();
  game.Tick();
  Point new_head = GetHead();
  EXPECT_EQ(new_head.x, old_head.x);
  EXPECT_EQ(new_head.y, old_head.y - 1);
}

TEST_F(SnakeGameTest, AccelerateMovesTwoSteps) {
  Point old_head = GetHead();
  game.Accelerate();
  game.Tick();
  Point new_head = GetHead();
  EXPECT_EQ(new_head.x, old_head.x);
  EXPECT_EQ(new_head.y, old_head.y - 2);
}

TEST_F(SnakeGameTest, EatingAppleIncreasesScoreAndLength) {
  Point head = GetHead();
  Point apple_pos = head;
  apple_pos.y -= 1;
  game.SetApple(apple_pos);

  int old_score = game.GetScore();
  int old_length = (int)game.GetSnake().size();

  game.Tick();

  EXPECT_EQ(game.GetScore(), old_score + 1);
  EXPECT_EQ((int)game.GetSnake().size(), old_length + 1);
}

TEST_F(SnakeGameTest, CollisionWithWallCausesLose) {
  game.SetSnake({{5, 0}});
  game.ChangeDirection(Direction::UP);
  game.Tick();
  EXPECT_EQ(game.GetState(), GameFSM::State::LOSE);
}

TEST_F(SnakeGameTest, CollisionWithSelfCausesLose) {
  std::deque<Point> snake = {{5, 5}, {5, 6}, {6, 6}, {6, 5}, {6, 4},
                             {5, 4}, {4, 4}, {4, 5}, {4, 6}};
  game.SetSnake(snake);
  game.ChangeDirection(Direction::RIGHT);
  game.Tick();
  EXPECT_EQ(game.GetState(), GameFSM::State::LOSE);
}

TEST_F(SnakeGameTest, PauseAndResumeGame) {
  game.Pause();
  EXPECT_EQ(game.GetState(), GameFSM::State::PAUSED);

  Point pos_before = GetHead();
  game.Tick();
  Point pos_after = GetHead();
  EXPECT_EQ(pos_before.x, pos_after.x);
  EXPECT_EQ(pos_before.y, pos_after.y);

  game.Resume();
  EXPECT_EQ(game.GetState(), GameFSM::State::RUNNING);
  game.Tick();
  Point pos_after_resume = GetHead();
  EXPECT_NE(pos_after.y, pos_after_resume.y);
}

TEST_F(SnakeGameTest, RestartResetsGameState) {
  game.Pause();
  game.SetSnake({{1, 1}});
  s21::Point p = {2, 2};
  game.SetApple(p);
  game.Restart();

  EXPECT_EQ(game.GetState(), GameFSM::State::RUNNING);
  EXPECT_EQ(game.GetScore(), 0);
  EXPECT_EQ(game.GetLevel(), 1);
  EXPECT_GE(game.GetTickDelay(), 100);
  EXPECT_LE(game.GetTickDelay(), 300);

  EXPECT_GE(game.GetSnake().size(), 4);

  Point apple = game.GetApple();
  EXPECT_GE(apple.x, 0);
  EXPECT_LT(apple.x, game.GetWidth());
  EXPECT_GE(apple.y, 0);
  EXPECT_LT(apple.y, game.GetHeight());
}

TEST_F(SnakeGameTest, SetAndGetState) {
  game.SetState(GameFSM::State::PAUSED);
  EXPECT_EQ(game.GetState(), GameFSM::State::PAUSED);
  game.SetState(GameFSM::State::WIN);
  EXPECT_EQ(game.GetState(), GameFSM::State::WIN);
}

TEST_F(SnakeGameTest, LevelAndTickDelayIncreaseWithScore) {
  const int target_score = 25;

  std::deque<s21::Point> snake = {{5, 10}, {5, 11}, {5, 12}, {5, 13}};
  game.SetSnake(snake);

  game.ChangeDirection(s21::Direction::UP);
  game.Tick();

  for (int i = 0; i < target_score; ++i) {
    s21::Point head = game.GetSnake().front();
    s21::Point apple_pos = {head.x, head.y - 1};

    if (apple_pos.y < 0) {
      for (auto &p : snake) {
        p.y += 10;
      }
      game.SetSnake(snake);
      head = game.GetSnake().front();
      apple_pos = {head.x, head.y - 1};
    }

    game.SetApple(apple_pos);
    game.Tick();
    if (game.GetState() != s21::GameFSM::State::RUNNING) break;
  }

  EXPECT_EQ(game.GetScore(), target_score);
  EXPECT_GE(game.GetLevel(), 5);
  EXPECT_LE(game.GetTickDelay(), 300);
  EXPECT_GE(game.GetTickDelay(), 100);
  EXPECT_EQ(game.GetState(), s21::GameFSM::State::RUNNING);
}

TEST_F(SnakeGameTest, HighScoreNeverDecreases) {
  int old_high = game.GetHighScore();
  game.Restart();

  for (int i = 0; i < 10; ++i) {
    Point head = GetHead();
    Point apple = head;
    apple.y -= 1;
    game.SetApple(apple);
    game.Tick();
    if (game.GetState() != GameFSM::State::RUNNING) break;
  }

  EXPECT_GE(game.GetHighScore(), old_high);
}

TEST_F(SnakeGameTest, HighScorePersistsAcrossInstances) {
  SnakeGame g1;
  int persisted = g1.GetHighScore();

  SnakeGame g2;
  EXPECT_EQ(g2.GetHighScore(), persisted);
}

TEST_F(SnakeGameTest, WinWhenSnakeReachesLength200) {
  std::deque<Point> long_snake;
  for (int i = 0; i < 199; ++i) {
    long_snake.push_back({5, 10 + i});
  }
  game.SetSnake(long_snake);

  Point head = game.GetSnake().front();
  Point apple_pos = {head.x, head.y - 1};
  game.SetApple(apple_pos);

  game.ChangeDirection(Direction::UP);

  game.Tick();

  EXPECT_EQ(game.GetState(), GameFSM::State::WIN);
  EXPECT_GE(game.GetSnake().size(), 200);
}
