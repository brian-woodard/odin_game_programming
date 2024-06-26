#include <iostream>
#include "raylib.h"

int main()
{
   Rectangle paddle = { 100, 100, 180, 30 };
   float paddle_speed = 10;

   Rectangle ball = { 100, 150, 30, 30 };
   Vector2 ball_dir = { 0, -1 };
   float ball_speed = 10;

   InitWindow(1280, 720, "Pong");
   SetTargetFPS(60);

   while (!WindowShouldClose())
   {
      if (IsKeyDown(KEY_A))
         paddle.x -= paddle_speed;
      if (IsKeyDown(KEY_D))
         paddle.x += paddle_speed;

      Rectangle next_ball_rect = ball;
      next_ball_rect.y += ball_speed * ball_dir.y;

      if (next_ball_rect.y >= 720 - ball.height || next_ball_rect.y <= 0)
      {
         ball_dir.y *= -1;
      }

      if (CheckCollisionRecs(next_ball_rect, paddle))
      {
         ball_dir.y *= -1;
      }

      ball.y += ball_speed * ball_dir.y;
             
      ClearBackground(BLACK);
      BeginDrawing();

      DrawRectangleRec(paddle, WHITE);
      DrawRectangleRec(ball, RED);

      EndDrawing();
   }

   CloseWindow();
}
