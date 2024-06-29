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
   Rectangle ai_paddle;
   float     ai_target_y;
   float     ai_reaction_delay;
   float     ai_reaction_timer;
   float     paddle_speed;
   Rectangle ball;
   Vector2   ball_dir;
   float     ball_speed;
   int       score_player;
   int       score_cpu;
   float     boost_timer;
   float     boost_time;
};

int get_rand_value()
{
   static bool first_pass = true;

   if (first_pass)
   {
      FILE*        random_fd = fopen("/dev/urandom", "rb");
      unsigned int seed;

      if (random_fd)
      {
         fread(&seed, sizeof(seed), 1, random_fd);
         fclose(random_fd);
         srand(seed);
      }

      first_pass = false;
   }

   return rand();
}

void reset(Game_State& gs)
{
   float angle;
   float r;
   float paddle_margin = 50.0;
   int   rand_value;

   rand_value = get_rand_value();
   angle = (((float)rand_value / (float)RAND_MAX) * 90.0f) - 45.0f;

   rand_value = get_rand_value();
   int direction = ((int)(((float)rand_value / (float)RAND_MAX) * 100.0f)) % 2;
   if (direction == 0)
      angle += 180.0;
   
   r = angle * M_PI / 180.0;

   gs.ball_dir.x = cos(r);
   gs.ball_dir.y = sin(r);

   gs.ball.x = gs.window_size.x / 2 + gs.ball.width / 2;
   gs.ball.y = gs.window_size.y / 2 + gs.ball.height / 2;
   
   gs.paddle.x = gs.window_size.x - (gs.paddle.width + paddle_margin);
   gs.paddle.y = gs.window_size.y / 2 - gs.paddle.height / 2;

   gs.ai_paddle.x = paddle_margin;
   gs.ai_paddle.y = gs.window_size.y / 2 - gs.paddle.height / 2;
}

bool ball_dir_calculate(Rectangle ball, Rectangle paddle, Vector2& new_ball_dir)
{
   if (CheckCollisionRecs(ball, paddle))
   {
      Vector2 ball_center = { ball.x + ball.width / 2, ball.y + ball.height / 2 };
      Vector2 paddle_center = { paddle.x + paddle.width / 2, paddle.y + paddle.height / 2 };
      new_ball_dir = Vector2Normalize(Vector2Subtract(ball_center, paddle_center));
      return true;
   }
   else
   {
      new_ball_dir.x = 0.0;
      new_ball_dir.y = 0.0;
      return false;
   }
}

int main()
{
   Game_State gs =
   {
      { 1280, 720 },
      { 0, 0, 30, 80 },
      { 0, 0, 30, 80 },
      0.0,
      0.1,
      0.0,
      10,
      { 0, 0, 30, 30 },
      { 0, 0 },
      10,
      0,
      0,
      0.0,
      1.0
   };

   reset(gs);

   InitWindow(gs.window_size.x, gs.window_size.y, "Pong");
   SetTargetFPS(60);

   InitAudioDevice();

   Sound sfx_hit = LoadSound("hit.wav");
   Sound sfx_win = LoadSound("win.wav");
   Sound sfx_lose = LoadSound("lose.wav");

   while (!WindowShouldClose())
   {
      float delta = GetFrameTime();

      gs.boost_timer -= delta;

      if (IsKeyDown(KEY_UP))
         gs.paddle.y -= gs.paddle_speed;
      if (IsKeyDown(KEY_DOWN))
         gs.paddle.y += gs.paddle_speed;
      if (IsKeyPressed(KEY_SPACE))
      {
         if (gs.boost_timer < 0.0)
         {
            gs.boost_timer = gs.boost_time;
            PlaySound(sfx_lose);
         }
      }

      // AI movement
      gs.ai_reaction_timer += delta;
      if (gs.ai_reaction_timer >= gs.ai_reaction_delay)
      {
         gs.ai_reaction_timer = 0.0;
         float ball_mid = gs.ball.y + gs.ball.height / 2.0;;
         if (gs.ball_dir.x < 0)
         {
            int rand_value = get_rand_value();
            float inaccuracy = (((float)rand_value / (float)RAND_MAX) * 40.0f) - 20.0f;
            gs.ai_target_y = ball_mid - gs.ai_paddle.height / 2.0;
            gs.ai_target_y += inaccuracy;
         }
         else
         {
            gs.ai_target_y = gs.window_size.y / 2.0 - gs.ai_paddle.height / 2.0;
         }
      }

      float ai_paddle_mid = gs.ai_paddle.y + gs.ai_paddle.height / 2.0;
      float target_diff = gs.ai_target_y - gs.ai_paddle.y;
      gs.ai_paddle.y += Clamp(target_diff, -gs.paddle_speed, gs.paddle_speed) * 0.65;
      gs.ai_paddle.y = Clamp(gs.ai_paddle.y, 0, gs.window_size.y - gs.ai_paddle.height);

      gs.paddle.y = Clamp(gs.paddle.y, 0, gs.window_size.y - gs.paddle.height);

      Rectangle next_ball_rect = gs.ball;
      next_ball_rect.x += gs.ball_speed * gs.ball_dir.x;
      next_ball_rect.y += gs.ball_speed * gs.ball_dir.y;

      if (next_ball_rect.x >= gs.window_size.x - gs.ball.width)
      {
         gs.score_cpu += 1;
         PlaySound(sfx_lose);
         reset(gs);
      }
      if (next_ball_rect.x <= 0)
      {
         gs.score_player += 1;
         PlaySound(sfx_win);
         reset(gs);
      }

      if (next_ball_rect.y >= gs.window_size.y - gs.ball.height || next_ball_rect.y <= 0)
      {
         PlaySound(sfx_hit);
         gs.ball_dir.y *= -1;
      }

      Vector2 new_dir;
      if (ball_dir_calculate(next_ball_rect, gs.paddle, new_dir))
      {
         if (gs.boost_timer > 0.0)
         {
            float boost = 1.0 + gs.boost_timer / gs.boost_time;
            new_dir.x *= boost;
            new_dir.y *= boost;
         }
         gs.ball_dir = new_dir;
         PlaySound(sfx_hit);
      }
      else if (ball_dir_calculate(next_ball_rect, gs.ai_paddle, new_dir))
      {
         gs.ball_dir = new_dir;
         PlaySound(sfx_hit);
      }

      gs.ball.x += gs.ball_speed * gs.ball_dir.x;
      gs.ball.y += gs.ball_speed * gs.ball_dir.y;
             
      ClearBackground(BLACK);
      BeginDrawing();

      if (gs.boost_timer > 0.0)
      {
         DrawRectangleRec(gs.paddle, { (unsigned char)(255.0 * (gs.boost_time - gs.boost_timer) / gs.boost_time), 255, 255, 255 });
      }
      else
      {
         DrawRectangleRec(gs.paddle, WHITE);
      }
      DrawRectangleRec(gs.ai_paddle, WHITE);
      DrawRectangleRec(gs.ball, { 255, (unsigned char)(255.0 - 255.0 / Vector2Length(gs.ball_dir)), 0, 255 });

      char score[5] = {};
      sprintf(score, "%d", gs.score_cpu);
      DrawText(score, 12, 12, 32, WHITE);
      sprintf(score, "%d", gs.score_player);
      DrawText(score, gs.window_size.x - 28, 12, 32, WHITE);

      EndDrawing();
   }

   CloseWindow();
   CloseAudioDevice();
}
