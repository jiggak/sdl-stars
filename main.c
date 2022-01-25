#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define STAR_COUNT 2000
#define STAR_SPREAD 4000

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif
#define RAD (M_PI/180)
#define PER 256

typedef struct Star
{
   float x, y, z;
   int dx, dy, col;
} Star;

void random_stars(Star *stars, int count) {
   for (int i = 0; i < count; i++) {
      stars[i].x = (rand() % STAR_SPREAD) - (STAR_SPREAD / 2);
      stars[i].y = (rand() % STAR_SPREAD) - (STAR_SPREAD / 2);
      stars[i].z = (rand() % STAR_SPREAD) - (STAR_SPREAD / 2);
      int tmp = rand() % 90;
      float temp1 = (stars[i].x * cos (tmp * RAD)) - (stars[i].z * sin (tmp * RAD));
      float temp2 = (stars[i].x * sin (tmp * RAD)) + (stars[i].z * cos (tmp * RAD));
      float temp3 = (stars[i].y * cos (tmp * RAD)) - (temp2 * sin (tmp * RAD));
      float temp4 = (stars[i].y * sin (tmp * RAD)) + (temp2 * cos (tmp * RAD));
      stars[i].x = temp1;
      stars[i].y = temp3;
      stars[i].z = temp4;
   }
}

void rotate_stars(Star *stars, int count, float yaw, float pitch, float roll) {
   // what do these do?
   const float xoff = 0.0f, yoff = 0.0f, zoff = 3200.0f;

   for (int i = 0; i < count; i++) {
      float temp1 = (stars[i].x * cos (yaw * RAD)) - (stars[i].z * sin (yaw * RAD));
      float temp2 = (stars[i].x * sin (yaw * RAD)) + (stars[i].z * cos (yaw * RAD));
      float temp3 = (stars[i].y * cos (pitch * RAD)) - (temp2 * sin (pitch * RAD));
      float temp4 = (stars[i].y * sin (pitch * RAD)) + (temp2 * cos (pitch * RAD));
      float temp5 = (temp1 * cos (roll * RAD)) - (temp3 * sin (roll * RAD));
      float temp6 = (temp1 * sin (roll * RAD)) + (temp3 * cos (roll * RAD));
      stars[i].dx = (PER * temp5 + xoff) / (PER + temp4 + zoff) + (SCREEN_WIDTH / 2);
      stars[i].dy = (PER * temp6 + yoff) / (PER + temp4 + zoff) + (SCREEN_HEIGHT / 2);

      stars[i].col = (temp4 + (STAR_SPREAD / 2)) * 0xff / STAR_SPREAD;
      //stars[i].col = ((STAR_SPREAD - temp4) + (STAR_SPREAD / 2)) * 0xff / STAR_SPREAD;

      // see[n] = per + temp4 + zoff;
      // if (see[n] > 0) {
      //    stars[n].col = 24 - temp4 / 250;
      // }
   }
}

void render(SDL_Renderer *ctx) {
   Star stars[STAR_COUNT];

   random_stars(stars, STAR_COUNT);

   SDL_bool running = SDL_TRUE;
   float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;

   while (running) {
      rotate_stars(stars, STAR_COUNT, yaw, pitch, roll);

      for (int i = 0; i < STAR_COUNT; i++) {
         SDL_SetRenderDrawColor(ctx, stars[i].col, stars[i].col, 0, 0xff);
         SDL_RenderDrawPoint(ctx, stars[i].dx, stars[i].dy);
      }

      yaw += 1;
      //pitch += 1;
      roll += 1;

      SDL_RenderPresent(ctx);

      SDL_Delay(50);

      SDL_SetRenderDrawColor(ctx, 0, 0, 0, 0xff);
      SDL_RenderClear(ctx);

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         switch (event.type) {
            case SDL_QUIT:
               running = SDL_FALSE;
               break;
         }
      }
   }
}

int main(void) {
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
      return 1;
   }

   SDL_Window *window = SDL_CreateWindow(
      "Stars",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN
   );

   if (window == NULL) {
      fprintf(stderr, "could not create window: %s\n", SDL_GetError());
      return 1;
   }

   SDL_Renderer *ctx = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   if (ctx == NULL) {
      fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
      return 1;
   }

   render(ctx);

   SDL_DestroyRenderer(ctx);
   SDL_DestroyWindow(window);
   SDL_Quit();

   return 0;
}

