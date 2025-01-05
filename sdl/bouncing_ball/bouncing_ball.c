#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 900;
const int TRAIL_LENGTH = 5;
const float TRAIL_SPACING = 60.0f;

struct Circle {
  int x, y;
  int r;
  int vx, vy;
};

struct TrailCircle {
  int x, y;
  int r;
};

struct TrailManager {
  struct TrailCircle *trail;
  int length;
  float spacing;
  float distance_since_last_trail;
  float velocity_magnitude;
};

int init(void) {
  int sdlInit = SDL_Init(SDL_INIT_VIDEO);
  if (sdlInit != 0) {
    fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
  }

  return sdlInit;
}

void cleanup(SDL_Window *win, struct TrailCircle *trail) {
  if (trail != NULL) {
    free(trail);
  }

  if (win != NULL) {
    SDL_DestroyWindow(win);
  }

  SDL_Quit();
}

void drawCircle(SDL_Surface *layer, struct Circle *circle, SDL_Color color) {
  if (SDL_MUSTLOCK(layer)) {
    if (SDL_LockSurface(layer) < 0) {
      fprintf(stderr, "SDL_LockSurface Error: %s\n", SDL_GetError());
      return;
    }
  }

  int rSq = circle->r * circle->r;
  Uint32 mappedColor = SDL_MapRGB(layer->format, color.r, color.g, color.b);
  int xStart = (circle->x - circle->r) < 0 ? 0 : circle->x - circle->r;
  int xEnd = (circle->x + circle->r) > SCREEN_WIDTH ? SCREEN_WIDTH
                                                    : circle->x + circle->r;
  int yStart = (circle->y - circle->r) < 0 ? 0 : circle->y - circle->r;
  int yEnd = (circle->y + circle->r) > SCREEN_HEIGHT ? SCREEN_HEIGHT
                                                     : circle->y + circle->r;

  for (int x = xStart; x < xEnd; x++) {
    for (int y = yStart; y < yEnd; y++) {
      int xDiff = x - circle->x;
      int yDiff = y - circle->y;
      int distSq = xDiff * xDiff + yDiff * yDiff;
      if (distSq <= rSq) {
        ((Uint32 *)layer->pixels)[y * layer->w + x] = mappedColor;
      }
    }
  }

  if (SDL_MUSTLOCK(layer)) {
    SDL_UnlockSurface(layer);
  }
}

void updateCircle(struct Circle *circle) {
  circle->x += circle->vx;
  circle->y += circle->vy;

  if (circle->x - circle->r < 0 || circle->x + circle->r > SCREEN_WIDTH) {
    circle->vx = -circle->vx;
    if (circle->x - circle->r < 0)
      circle->x = circle->r;
    if (circle->x + circle->r > SCREEN_WIDTH)
      circle->x = SCREEN_WIDTH - circle->r;
  }

  if (circle->y - circle->r < 0 || circle->y + circle->r > SCREEN_HEIGHT) {
    circle->vy = -circle->vy;
    if (circle->y - circle->r < 0)
      circle->y = circle->r;
    if (circle->y + circle->r > SCREEN_HEIGHT)
      circle->y = SCREEN_HEIGHT - circle->r;
  }
}

void initializeTrail(struct TrailManager *manager, struct Circle *circle) {
  manager->trail = malloc(manager->length * sizeof(struct TrailCircle));
  if (manager->trail == NULL) {
    fprintf(stderr, "Failed to allocate memory for trail.\n");
    exit(1);
  }

  manager->velocity_magnitude = sqrt(pow(circle->vx, 2) + pow(circle->vy, 2));

  for (int i = 0; i < manager->length; i++) {
    if (manager->velocity_magnitude == 0) {
      manager->trail[i].x = circle->x;
      manager->trail[i].y = circle->y;
    } else {
      manager->trail[i].x =
          circle->x - (circle->vx * manager->spacing * (i + 1)) /
                          manager->velocity_magnitude;
      manager->trail[i].y =
          circle->y - (circle->vy * manager->spacing * (i + 1)) /
                          manager->velocity_magnitude;
    }
    manager->trail[i].r = circle->r - ((i + 1) * 4); // Shrinking radius
    if (manager->trail[i].r < 0)
      manager->trail[i].r = 0;
  }

  manager->distance_since_last_trail = 0.0f;
}

void addTrail(struct TrailManager *manager, struct Circle *circle) {
  for (int i = manager->length - 1; i > 0; i--) {
    manager->trail[i] = manager->trail[i - 1];
    manager->trail[i].r = circle->r - (i * 4);
    if (manager->trail[i].r < 0)
      manager->trail[i].r = 0;
  }

  manager->trail[0].x = circle->x;
  manager->trail[0].y = circle->y;
  manager->trail[0].r = circle->r;
}

void drawTrail(SDL_Surface *layer, struct TrailCircle *trail, int length) {
  for (int i = 0; i < length; i++) {
    int alpha = 255 - (i * 20);
    SDL_Color trailColor = {alpha, alpha, 0, 255};
    struct Circle temp = {(int)trail[i].x, (int)trail[i].y, trail[i].r, 0, 0};
    drawCircle(layer, &temp, trailColor);
  }
}

void drawUpdate(SDL_Surface *layer, SDL_Window *win, struct Circle *circle,
                struct TrailManager *manager) {
  SDL_FillRect(layer, NULL, SDL_MapRGB(layer->format, 0, 0, 0));

  SDL_Color circleColor = {255, 255, 255, 255};
  drawTrail(layer, manager->trail, manager->length);
  drawCircle(layer, circle, circleColor);

  SDL_UpdateWindowSurface(win);
}

int main(void) {
  if (init() != 0)
    return 1;

  SDL_Window *win = SDL_CreateWindow(
      "Bouncing Ball with Trail", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

  struct Circle circle = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 50, 5, 5};

  struct TrailManager manager;
  manager.length = TRAIL_LENGTH;
  manager.spacing = TRAIL_SPACING;
  initializeTrail(&manager, &circle);

  SDL_Event e;
  int running = 1;
  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = 0;
        break;
      }

      if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        running = 0;
        break;
      }
    }

    if (!running)
      break;

    updateCircle(&circle);

    float dx = circle.vx;
    float dy = circle.vy;
    float distance_moved = sqrtf(dx * dx + dy * dy);
    manager.distance_since_last_trail += distance_moved;

    if (manager.distance_since_last_trail >= manager.spacing) {
      addTrail(&manager, &circle);
      manager.distance_since_last_trail -= manager.spacing;
    }

    drawUpdate(screen, win, &circle, &manager);
    SDL_Delay(1000 / 120);
  }

  cleanup(win, manager.trail);
  return 0;
}
