#ifndef TETRIS_GAME_HPP
#define TETRIS_GAME_HPP

#include <cstdbool>

extern "C"
{
#include "tetris_c_api.h"
}

namespace s21
{

    class TetrisGame
    {
    public:
        TetrisGame();

        void StartGame();
        void TogglePause();
        void TerminateGame();

        void MoveLeft();
        void MoveRight();
        void MoveDown();
        void Rotate();

        void Update();

        int GetScore() const;
        int GetHighScore() const;
        int GetLevel() const;
        int GetSpeed() const;
        bool IsPaused() const;

        int **GetField() const { return state_.field; }
        int GetWidth() const { return FIELD_WIDTH; }
        int GetHeight() const { return FIELD_HEIGHT; }
        int **GetNextMatrix() const { return state_.next; }

    private:
        void UpdateState();

        GameInfo_t state_;
    };

} // namespace s21

#endif // TETRIS_GAME_HPP
