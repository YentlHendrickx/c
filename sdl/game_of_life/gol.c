#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>
#include <stdlib.h>

// I don't like it being odd, but it's easier to contain
const int SCREEN_WIDTH = 1501;
const int SCREEN_HEIGHT = 1201;
const int CELL_SIZE = 10;

struct Cell {
  int x;
  int y;
  int state;
  struct Cell *neighbors[8];
  SDL_Color color;
};

int init(void) {
  int sdlInit = SDL_Init(SDL_INIT_VIDEO);
  if (sdlInit != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
  }

  return sdlInit;
}

void cleanup(SDL_Window *win, struct Cell *cells) {
  if (cells != NULL) {
    free(cells);
  }

  if (win != NULL) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
}

void drawCells(SDL_Surface *screen, struct Cell *cells, int toggleColor) {
  SDL_Rect rect;
  rect.w = CELL_SIZE - 1;
  rect.h = CELL_SIZE - 1;

  int rows = SCREEN_HEIGHT / CELL_SIZE;
  int cols = SCREEN_WIDTH / CELL_SIZE;

  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 128, 128, 128));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      rect.x = j * CELL_SIZE + 1;
      rect.y = i * CELL_SIZE + 1;

      int index = i * cols + j;
      if (cells[index].state == 1) {
        int red = 255;
        int green = 255;
        int blue = 255;

        if (toggleColor) {
          red = cells[index].color.r;
          green = cells[index].color.g;
          blue = cells[index].color.b;
        }

        SDL_FillRect(screen, &rect,
                     SDL_MapRGB(screen->format, red, green, blue));
      } else if (cells[index].state == 0) {
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
      } else {
        SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 255));
      }
    }
  }
}

void toggleCellState(struct Cell *cells, int x, int y) {
  int cols = SCREEN_WIDTH / CELL_SIZE;

  int cellX = x / CELL_SIZE;
  int cellY = y / CELL_SIZE;

  int index = cellY * cols + cellX;
  cells[index].state = !cells[index].state;
}

void showNeighbors(struct Cell *cells, int x, int y) {
  int cols = SCREEN_WIDTH / CELL_SIZE;

  // Get the cell
  int cellX = x / CELL_SIZE;
  int cellY = y / CELL_SIZE;

  int index = cellY * cols + cellX;
  struct Cell *cell = &cells[index];

  for (int i = 0; i < 8; i++) {
    if (cell->neighbors[i] == NULL) {
      continue;
    }

    cell->neighbors[i]->state = 3;
  }
}

void clearCells(struct Cell *cells) {
  int rows = SCREEN_HEIGHT / CELL_SIZE;
  int cols = SCREEN_WIDTH / CELL_SIZE;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int index = i * cols + j;
      cells[index].state = 0;
    }
  }
}

int nextCellState(struct Cell *cell) {
  int aliveNeighbors = 0;
  for (int i = 0; i < 8; i++) {
    if (cell->neighbors[i] == NULL) {
      continue;
    }

    if (cell->neighbors[i]->state)
      aliveNeighbors++;
  }

  if (cell->state) {
    if (aliveNeighbors < 2 || aliveNeighbors > 3)
      return 0;
  } else {
    if (aliveNeighbors == 3)
      return 1;
  }

  return cell->state;
}

struct Cell *getCellsToCheck(struct Cell *cells, int rows, int x, int y) {
  // Return the current cells index, and all its neighbors so we can check them
  struct Cell *cellsToCheck = malloc(8 * sizeof(struct Cell));
  struct Cell *currentCell = &cells[y * rows + x];

  for (int i = 0; i < 8; i++) {
    if (currentCell->neighbors[i] == NULL) {
      cellsToCheck[i].state = -1;
      continue;
    }

    cellsToCheck[i] = *currentCell->neighbors[i];
  }

  return cellsToCheck;
}

int inCheckSet(struct Cell *cells, struct Cell *cell, int rows, int cols) {
  for (int i = 0; i < rows * cols; i++) {
    if (cells[i].state == -1) {
      return 0;
    }

    if (cells[i].x == cell->x && cells[i].y == cell->y) {
      return 1;
    }
  }

  return 0;
}

struct Cell *nextGeneration(struct Cell *cells, int rows, int cols) {
  // Make a copy of the cells, so we can update them all at once -> otherwise
  // invalidates the neighbors
  struct Cell *nextGen = malloc(rows * cols * sizeof(struct Cell));
  struct Cell *cellsToCheck = malloc(rows * cols * sizeof(struct Cell));
  // Initialize cells to check to have state -1
  for (int i = 0; i < rows * cols; i++) {
    cellsToCheck[i].state = -1;
  }
  int currentIndex = 0;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int index = i * cols + j;
      nextGen[index] = cells[index];

      if (cells[index].state <= 0) {
        continue;
      }

      if (!inCheckSet(cellsToCheck, &cells[index], rows, cols)) {
        cellsToCheck[currentIndex] = cells[index];
        currentIndex++;
      }

      struct Cell *neighbors =
          getCellsToCheck(cells, cols, cells[index].x, cells[index].y);

      for (int j = 0; j < 8; j++) {
        // Skip NULL neighbors
        if (neighbors[j].state == -1) {
          continue;
        }

        // Check if neighbor is already in the list
        if (inCheckSet(cellsToCheck, &neighbors[j], rows, cols) == 1) {
          continue;
        }

        cellsToCheck[currentIndex] = neighbors[j];
        currentIndex++;
      }
    }
  }

  for (int i = 0; i < currentIndex; i++) {
    int nextState = nextCellState(&cellsToCheck[i]);
    int index = cellsToCheck[i].y * cols + cellsToCheck[i].x;
    nextGen[index].state = nextState;
  }

  free(cellsToCheck);
  return nextGen;
}

void setNeighbors(struct Cell *cells, int y, int x, int rows, int cols) {
  int neighbors[8][2] = {
      {y - 1, x - 1}, {y - 1, x},     {y - 1, x + 1}, {y, x - 1},
      {y, x + 1},     {y + 1, x - 1}, {y + 1, x},     {y + 1, x + 1},
  };

  int current = y * cols + x;

  for (int i = 0; i < 8; i++) {
    int neighborY = neighbors[i][0];
    int neighborX = neighbors[i][1];

    if (neighborY < 0 || neighborY >= rows || neighborX < 0 ||
        neighborX >= cols)
      continue;

    int neighbor = neighborY * cols + neighborX;
    cells[current].neighbors[i] = &cells[neighbor];
  }
}

void randomizeCells(struct Cell *cells, int rows, int cols) {
  for (int i = 0; i < rows * cols; i++) {
    // 75% chance of being dead
    int chance = rand() % 100;
    if (chance < 75) {
      cells[i].state = 0;
    } else {
      cells[i].state = 1;
    }
  }
}

void initializeCells(struct Cell *cells, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      setNeighbors(cells, i, j, rows, cols);
      cells[i * cols + j].x = j;
      cells[i * cols + j].y = i;

      int r = rand() % 2 == 0 ? 0 : 255;
      int g = rand() % 2 == 0 ? 0 : 255;
      int b = rand() % 2 == 0 ? 0 : 255;

      cells[i * cols + j].color = (SDL_Color){r, g, b, 1};
      /* cells[i * cols + j].color = (SDL_Color){255, 255, 255, 1}; */
    }
  }
}

int main(void) {
  if (init() != 0)
    return 1;

  SDL_Window *win = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                     SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

  if (win == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    cleanup(win, NULL);
    return 1;
  }

  SDL_Surface *screen = SDL_GetWindowSurface(win);
  if (screen == NULL) {
    fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    cleanup(win, NULL);
    return 1;
  }

  int rows = SCREEN_HEIGHT / CELL_SIZE;
  int cols = SCREEN_WIDTH / CELL_SIZE;
  printf("Rows: %d, Cols: %d\n", rows, cols);
  struct Cell *cells = malloc(rows * cols * sizeof(struct Cell));
  struct Cell *nextCells = malloc(rows * cols * sizeof(struct Cell));
  initializeCells(cells, rows, cols);

  SDL_Event e;
  int exitTrigger = 0;
  int pause = 1;
  int speedDef = 1000;
  int speedDivisor = 100;
  int toggleColor = 0;
  while (!exitTrigger) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        exitTrigger = 1;
        break;
      }

      if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          exitTrigger = 1;
          break;
        // Toggle pause
        case SDLK_SPACE:
          pause = !pause;
          break;

        // Next generation
        case SDLK_RIGHT:
          nextCells = nextGeneration(cells, rows, cols);
          for (int i = 0; i < rows * cols; i++) {
            cells[i] = nextCells[i];
          }
          free(nextCells);
          break;

        case SDLK_r:
          randomizeCells(cells, rows, cols);
          break;

        case SDLK_UP:
          speedDivisor -= 2;
          if (speedDivisor < 1) {
            speedDivisor = 1;
          }
          break;

        case SDLK_DOWN:
          speedDivisor += 2;
          if (speedDivisor > speedDef) {
            speedDivisor = speedDef;
          }
          break;

        case SDLK_t:
          toggleColor = !toggleColor;
          break;

        case SDLK_c:
          clearCells(cells);
          break;
        }
      }

      if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
          toggleCellState(cells, e.button.x, e.button.y);
        } else if (e.button.button == SDL_BUTTON_RIGHT) {
          showNeighbors(cells, e.button.x, e.button.y);
        }
      }
    }

    if (exitTrigger)
      break;

    if (!pause) {
      nextCells = nextGeneration(cells, rows, cols);
      for (int i = 0; i < rows * cols; i++) {
        cells[i] = nextCells[i];
      }
      free(nextCells);
    }

    drawCells(screen, cells, toggleColor);
    SDL_UpdateWindowSurface(win);

    if (pause) {
      SDL_Delay(1000 / 120);
    } else {
      SDL_Delay(speedDef / speedDivisor);
    }
  }

  cleanup(win, cells);
  return 0;
}
