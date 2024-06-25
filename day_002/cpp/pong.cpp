#include <iostream>
#include "raylib.h"

int main()
{
   int pos = 100;

   InitWindow(1280, 720, "Pong");
   SetTargetFPS(60);

   while (!WindowShouldClose())
   {
      if (IsKeyDown(KEY_A))
         pos -= 10;
      if (IsKeyDown(KEY_D))
         pos += 10;
             
      ClearBackground(BLACK);
      BeginDrawing();

      DrawRectangle(pos, 100, 180, 30, WHITE);

      EndDrawing();
   }

   CloseWindow();
}
