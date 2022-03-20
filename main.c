#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define STAR_COUNT 1000
#define STAR_SPREAD 5000

#define SCALE 600
#define DISTANCE 4000

#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif
#define RAD (M_PI/180)

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
   }
}

// https://stackoverflow.com/questions/34050929/3d-point-rotation-algorithm
void rotate_stars(Star *stars, int count, double pitch, double roll, double yaw) {
   double cosa = cos(yaw);
   double sina = sin(yaw);

   double cosb = cos(pitch);
   double sinb = sin(pitch);

   double cosc = cos(roll);
   double sinc = sin(roll);

   double Axx = cosa*cosb;
   double Axy = cosa*sinb*sinc - sina*cosc;
   double Axz = cosa*sinb*cosc + sina*sinc;

   double Ayx = sina*cosb;
   double Ayy = sina*sinb*sinc + cosa*cosc;
   double Ayz = sina*sinb*cosc - cosa*sinc;

   double Azx = -sinb;
   double Azy = cosb*sinc;
   double Azz = cosb*cosc;

   for (int i = 0; i < count; i++) {
      float px = stars[i].x;
      float py = stars[i].y;
      float pz = stars[i].z;

      stars[i].x = Axx*px + Axy*py + Axz*pz;
      stars[i].y = Ayx*px + Ayy*py + Ayz*pz;
      stars[i].z = Azx*px + Azy*py + Azz*pz;
   }
}

// http://anthony.liekens.net/index.php/Computers/RenderingTutorial3DTo2D
void project_stars(Star *stars, int count, int scale, int distance) {
   static float zmax = 0, zmin = 0;

   int xoff = SCREEN_WIDTH / 2;
   int yoff = SCREEN_HEIGHT / 2;

   for (int i = 0; i < count; i++) {
      stars[i].dx = xoff + scale * stars[i].x / ( stars[i].z + distance );
      stars[i].dy = yoff + scale * stars[i].y / ( stars[i].z + distance );

      zmax = fmax(zmax, stars[i].z);
      zmin = fmin(zmin, stars[i].z);

      stars[i].col = 255 - ((stars[i].z + fabs(zmin)) * 255) / (zmax + fabs(zmin));
   }
}

void render(SDL_Renderer *ctx) {
   Star stars[STAR_COUNT];

   random_stars(stars, STAR_COUNT);

   SDL_bool running = SDL_TRUE;
   float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
   int scale = SCALE, distance = DISTANCE;

   while (running) {
      rotate_stars(stars, STAR_COUNT, yaw, pitch, roll);
      project_stars(stars, STAR_COUNT, scale, distance);

      for (int i = 0; i < STAR_COUNT; i++) {
         SDL_SetRenderDrawColor(ctx, stars[i].col, stars[i].col, stars[i].col, 0xff);
         SDL_RenderDrawPoint(ctx, stars[i].dx, stars[i].dy);
      }

      SDL_RenderPresent(ctx);

      SDL_Delay(1000/60);

      SDL_SetRenderDrawColor(ctx, 0, 0, 0, 0xff);
      SDL_RenderClear(ctx);

      SDL_Event event;
      while (SDL_PollEvent(&event)) {
         switch (event.type) {
            case SDL_KEYUP:
               switch (event.key.keysym.sym) {
                  case SDLK_UP:
                     pitch = pitch == 0 ? -RAD : 0;
                     break;
                  case SDLK_DOWN:
                     pitch = pitch == 0 ? RAD : 0;
                     break;
                  case SDLK_LEFT:
                     yaw = yaw == 0 ? RAD : 0;
                     break;
                  case SDLK_RIGHT:
                     yaw = yaw == 0 ? -RAD : 0;
                     break;
                  case SDLK_1:
                     distance += 100;
                     break;
                  case SDLK_2:
                     distance -= 100;
                     break;
                  case SDLK_3:
                     scale += 10;
                     break;
                  case SDLK_4:
                     scale -= 10;
                     break;
               }

               printf("distance %d scale %d\n", distance, scale);

               break;
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

