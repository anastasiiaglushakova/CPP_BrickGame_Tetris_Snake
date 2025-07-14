#include "tetris_c_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>

static void GetHighScoreFilePath(char *buffer, size_t size) {
  char exe_path[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len != -1) {
    exe_path[len] = '\0';
    char *dir = dirname(exe_path);
    snprintf(buffer, size, "%s/tetris_highscore.txt", dir);
  } else {
    snprintf(buffer, size, "tetris_highscore.txt");  // fallback
  }
}
typedef enum
{
  START,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  PAUSE,
  GAMEOVER,
  EXIT
} FsmState_t;

typedef struct
{
  int shape[FIGURE_SIZE][FIGURE_SIZE];
  int x;
  int y;
} Piece_t;

typedef struct
{
  FsmState_t state;
  GameInfo_t info;
  Piece_t current_piece;
  Piece_t next_piece;
  int current_piece_index;
  int next_piece_index;
  int field[FIELD_HEIGHT][FIELD_WIDTH];
  bool game_over;
  int lines_cleared;
  unsigned long last_fall_time;
} Game_t;

static Game_t g_game = {0};

static const int kTetrominos[TETRIS_FIGURES_COUNT][FIGURE_SIZE][FIGURE_SIZE] = {
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}}};

static void InitializeGame(void);
static void SpawnPiece(void);
static bool CheckCollision(const Piece_t *piece);
static void FixPiece(void);
static void ClearLines(void);
static void RotatePiece(Piece_t *piece);
static void MovePiece(int dx, int dy);
static void UpdateGameField(void);
static int **AllocateMatrix(int rows, int cols);
static void FreeMatrix(int **matrix, int rows);
static unsigned long GetCurrentTimeMs(void);
static int GenerateRandomPiece(void);
static int LoadHighScore(void);
static void SaveHighScore(int score);

void userInput(UserAction_t action, bool hold)
{
  (void)hold;
  switch (g_game.state)
  {
  case START:
    if (action == Start)
    {
      InitializeGame();
    }
    else if (action == Terminate)
    {
      g_game.state = EXIT;
    }
    break;
  case MOVING:
    switch (action)
    {
    case Left:
      MovePiece(-1, 0);
      break;
    case Right:
      MovePiece(1, 0);
      break;
    case Down:
      while (!CheckCollision(&g_game.current_piece))
        g_game.current_piece.y++;
      g_game.current_piece.y--;
      g_game.state = ATTACHING;
      break;
    case Action:
      RotatePiece(&g_game.current_piece);
      break;
    case Pause:
      g_game.state = PAUSE;
      g_game.info.pause = true;
      break;
    case Terminate:
      g_game.state = START;
      break;
    default:
      break;
    }
    break;
  case PAUSE:
    if (action == Pause)
    {
      g_game.state = MOVING;
      g_game.info.pause = false;
    }
    else if (action == Terminate)
    {
      g_game.state = START;
      g_game.info.pause = false;
    }
    break;
  case GAMEOVER:

    if (action == Start)
    {
      InitializeGame();
    }
    else if (action == Terminate)
    {
      g_game.state = START;
    }
    break;
  default:
    break;
  }
}

GameInfo_t updateCurrentState()
{
  unsigned long current_time = GetCurrentTimeMs();
  switch (g_game.state)
  {
  case START:
    break;
  case SPAWN:
    SpawnPiece();
    break;
  case MOVING:
    if (current_time - g_game.last_fall_time >=
        (unsigned long)g_game.info.speed)
      g_game.state = SHIFTING;
    break;
  case SHIFTING:
    g_game.current_piece.y++;
    if (CheckCollision(&g_game.current_piece))
    {
      g_game.current_piece.y--;
      g_game.state = ATTACHING;
    }
    else
    {
      g_game.state = MOVING;
    }
    g_game.last_fall_time = current_time;
    break;
  case ATTACHING:
    FixPiece();
    ClearLines();
    g_game.state = SPAWN;
    break;
  case PAUSE:
    break;
  case GAMEOVER:
    g_game.game_over = true;
    break;
  case EXIT:
    break;
  }
  UpdateGameField();
  return g_game.info;
}

static void InitializeGame(void)
{
  if (g_game.info.field)
    FreeMatrix(g_game.info.field, FIELD_HEIGHT);
  if (g_game.info.next)
    FreeMatrix(g_game.info.next, FIGURE_SIZE);

  g_game.info.field = AllocateMatrix(FIELD_HEIGHT, FIELD_WIDTH);
  g_game.info.next = AllocateMatrix(FIGURE_SIZE, FIGURE_SIZE);

  if (!g_game.info.field || !g_game.info.next)
    return;

  g_game.info.score = 0;
  g_game.info.level = 1;
  g_game.info.speed = INITIAL_SPEED;
  g_game.info.pause = false;
  g_game.lines_cleared = 0;

  memset(g_game.field, 0, sizeof(g_game.field));

  srand(time(NULL));

  g_game.next_piece_index = GenerateRandomPiece();
  memcpy(g_game.next_piece.shape, kTetrominos[g_game.next_piece_index],
         sizeof(kTetrominos[g_game.next_piece_index]));
  g_game.next_piece.x = SPAWN_X;
  g_game.next_piece.y = SPAWN_Y;

  g_game.info.high_score = LoadHighScore();

  g_game.state = SPAWN;
  g_game.last_fall_time = GetCurrentTimeMs();
}

static void SpawnPiece(void)
{
  g_game.current_piece = g_game.next_piece;
  g_game.current_piece_index = g_game.next_piece_index;
  g_game.next_piece_index = GenerateRandomPiece();
  memcpy(g_game.next_piece.shape, kTetrominos[g_game.next_piece_index],
         sizeof(kTetrominos[g_game.next_piece_index]));
  g_game.next_piece.x = SPAWN_X;
  g_game.next_piece.y = SPAWN_Y;

  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      if (g_game.next_piece.shape[i][j] != 0)
        g_game.info.next[i][j] = g_game.next_piece_index + 1;
      else
        g_game.info.next[i][j] = 0;

  if (CheckCollision(&g_game.current_piece))
  {
    g_game.state = GAMEOVER;
    g_game.game_over = true;
  }
  else
  {
    g_game.state = MOVING;
  }
}

static bool CheckCollision(const Piece_t *piece)
{
  for (int i = 0; i < FIGURE_SIZE; i++)
  {
    for (int j = 0; j < FIGURE_SIZE; j++)
    {
      if (piece->shape[i][j])
      {
        int x = piece->x + j;
        int y = piece->y + i;
        if (x < 0 || x >= FIELD_WIDTH || y >= FIELD_HEIGHT)
          return true;
        if (y >= 0 && g_game.field[y][x])
          return true;
      }
    }
  }
  return false;
}

static void FixPiece(void)
{
  for (int i = 0; i < FIGURE_SIZE; i++)
  {
    for (int j = 0; j < FIGURE_SIZE; j++)
    {
      if (g_game.current_piece.shape[i][j])
      {
        int x = g_game.current_piece.x + j;
        int y = g_game.current_piece.y + i;
        if (x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT)
        {
          g_game.field[y][x] = g_game.current_piece_index + 1;
        }
      }
    }
  }
}

static void ClearLines(void)
{
  int lines_cleared = 0;
  for (int i = FIELD_HEIGHT - 1; i >= 0; i--)
  {
    int j = 0;
    while (j < FIELD_WIDTH && g_game.field[i][j])
      j++;
    if (j == FIELD_WIDTH)
    {
      for (int k = i; k > 0; k--)
        memcpy(g_game.field[k], g_game.field[k - 1], sizeof(g_game.field[k]));
      memset(g_game.field[0], 0, sizeof(g_game.field[0]));
      lines_cleared++;
      i++;
    }
  }

  if (!lines_cleared)
    return;

  g_game.lines_cleared += lines_cleared;
  static const int scores[] = {0, 100, 300, 700, 1500};
  g_game.info.score +=
      lines_cleared <= 4 ? scores[lines_cleared] : lines_cleared * 100;
  int level = g_game.info.score / 600 + 1;
  if (level > 10)
    level = 10;
  g_game.info.level = level;
  g_game.info.speed = INITIAL_SPEED - (g_game.info.level - 1) * SPEED_DECREASE;
  if (g_game.info.speed < MIN_SPEED)
    g_game.info.speed = MIN_SPEED;
  if (g_game.info.score > g_game.info.high_score)
  {
    g_game.info.high_score = g_game.info.score;
    SaveHighScore(g_game.info.high_score);
  }
}

static void RotatePiece(Piece_t *piece)
{
  int rotated[FIGURE_SIZE][FIGURE_SIZE];
  for (int i = 0; i < FIGURE_SIZE; i++)
    for (int j = 0; j < FIGURE_SIZE; j++)
      rotated[j][FIGURE_SIZE - 1 - i] = piece->shape[i][j];
  int backup[FIGURE_SIZE][FIGURE_SIZE];
  memcpy(backup, piece->shape, sizeof(backup));
  memcpy(piece->shape, rotated, sizeof(rotated));
  if (CheckCollision(piece))
    memcpy(piece->shape, backup, sizeof(backup));
}

static void MovePiece(int dx, int dy)
{
  g_game.current_piece.x += dx;
  g_game.current_piece.y += dy;
  if (CheckCollision(&g_game.current_piece))
  {
    g_game.current_piece.x -= dx;
    g_game.current_piece.y -= dy;
  }
}

static void UpdateGameField(void)
{
  if (!g_game.info.field)
    return;
  for (int i = 0; i < FIELD_HEIGHT; i++)
    for (int j = 0; j < FIELD_WIDTH; j++)
      g_game.info.field[i][j] = g_game.field[i][j];
  if (g_game.state == MOVING || g_game.state == SHIFTING)
  {
    for (int i = 0; i < FIGURE_SIZE; i++)
    {
      for (int j = 0; j < FIGURE_SIZE; j++)
      {
        if (g_game.current_piece.shape[i][j])
        {
          int x = g_game.current_piece.x + j;
          int y = g_game.current_piece.y + i;
          if (x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT)
            g_game.info.field[y][x] = g_game.current_piece_index + 1;
        }
      }
    }
  }
}

static int **AllocateMatrix(int rows, int cols)
{
  int **matrix = malloc(rows * sizeof(int *));
  if (!matrix)
    return NULL;
  for (int i = 0; i < rows; i++)
  {
    matrix[i] = calloc(cols, sizeof(int));
    if (!matrix[i])
    {
      for (int j = 0; j < i; j++)
        free(matrix[j]);
      free(matrix);
      return NULL;
    }
  }
  return matrix;
}

static void FreeMatrix(int **matrix, int rows)
{
  if (!matrix)
    return;
  for (int i = 0; i < rows; i++)
  {
    if (matrix[i])
      free(matrix[i]);
  }
  free(matrix);
}

static unsigned long GetCurrentTimeMs(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static int GenerateRandomPiece(void) { return rand() % TETRIS_FIGURES_COUNT; }

static int LoadHighScore(void)
{
  char path[PATH_MAX];
  GetHighScoreFilePath(path, sizeof(path));

  FILE *f = fopen(path, "r");
  int score = 0;
  if (f)
  {
    fscanf(f, "%d", &score);
    fclose(f);
  }
  return score;
}

static void SaveHighScore(int score)
{
  char path[PATH_MAX];
  GetHighScoreFilePath(path, sizeof(path));

  FILE *f = fopen(path, "w");
  if (f)
  {
    fprintf(f, "%d\n", score);
    fclose(f);
  }
}