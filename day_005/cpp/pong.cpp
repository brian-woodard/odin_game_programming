#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

struct Game_State
{
   Vector2   window_size;
   Rectangle paddle;
   float     paddle_speed;
   Rectangle ball;
   Vector2   ball_dir;
   float     ball_speed;
};

void reset(Game_State& gs)
{
   FILE*        random_fd = fopen("/dev/urandom", "rb");
   unsigned int seed;
   int          rand_value;
   float        angle;
   float        r;

   if (random_fd)
   {
      fread(&seed, sizeof(seed), 1, random_fd);
      fclose(random_fd);
      srand(seed);
   }

   rand_value = rand();
   angle = (((float)rand_value / (float)RAND_MAX) * 90.0f) - 45.0f;
   r = angle * M_PI / 180.0;

   gs.ball_dir.x = cos(r);
   gs.ball_dir.y = sin(r);

   gs.ball.x = gs.window_size.x / 2 + gs.ball.width / 2;
   gs.ball.y = gs.window_size.y / 2 + gs.ball.height / 2;
   
   gs.paddle.x = gs.window_size.x - 80;
   gs.paddle.y = gs.window_size.y / 2 + gs.paddle.height / 2;
}

int main()
{
   Game_State gs =
   {
      { 1280, 720 },
      { 0, 0, 30, 80 },
      10,
      { 0, 0, 30, 30 },
      { 0, 0 },
      10
   };

   reset(gs);

   printf(">>> ball speed %f\n", gs.ball_speed);

   InitWindow(gs.window_size.x, gs.window_size.y, "Pong");
   SetTargetFPS(60);

   while (!WindowShouldClose())
   {
      if (IsKeyDown(KEY_UP))
         gs.paddle.y -= gs.paddle_speed;
      if (IsKeyDown(KEY_DOWN))
         gs.paddle.y += gs.paddle_speed;

      gs.paddle.y = Clamp(gs.paddle.y, 0, gs.window_size.y - gs.paddle.height);

      Rectangle next_ball_rect = gs.ball;
      next_ball_rect.x += gs.ball_speed * gs.ball_dir.x;
      next_ball_rect.y += gs.ball_speed * gs.ball_dir.y;

      if (next_ball_rect.x >= gs.window_size.x - gs.ball.width || next_ball_rect.x <= 0)
      {
         reset(gs);
      }

      if (next_ball_rect.y >= gs.window_size.y - gs.ball.height || next_ball_rect.y <= 0)
      {
         gs.ball_dir.y *= -1;
      }

      if (CheckCollisionRecs(next_ball_rect, gs.paddle))
      {
         Vector2 ball_center = { next_ball_rect.x + gs.ball.width / 2, next_ball_rect.y + gs.ball.height / 2 };
         Vector2 paddle_center = { gs.paddle.x + gs.paddle.width / 2, gs.paddle.y + gs.paddle.height / 2 };
         gs.ball_dir = Vector2Normalize(Vector2Subtract(ball_center, paddle_center));
      }

      gs.ball.x += gs.ball_speed * gs.ball_dir.x;
      gs.ball.y += gs.ball_speed * gs.ball_dir.y;
             
      ClearBackground(BLACK);
      BeginDrawing();

      DrawRectangleRec(gs.paddle, WHITE);
      DrawRectangleRec(gs.ball, RED);

      EndDrawing();
   }

   CloseWindow();
}
