#include <iostream>
#include "raylib.h"

/*
x2 + y2 - r2 = 0

x2 + 2x + 1 + y2 - r2 = e0
x2 + y2 + 2x + 1 - r2 = e1

[x2 + y2 -r2 + 1] + 2x = e0
[x2 + y2 -r2 + 1] + 2y = e1
*/

void DrawCircle(int X, int Y, int Radius, Color CircleColor)
{
   int cx = -Radius;
   int cy = 0;
   int error = 0;

   while (-cx > cy)
   {
      DrawPixel(X + cx, Y + cy, CircleColor);
      DrawPixel(X + cx, Y - cy, CircleColor);
      DrawPixel(X - cx, Y + cy, CircleColor);
      DrawPixel(X - cx, Y - cy, CircleColor);
      DrawPixel(X + cy, Y + cx, CircleColor);
      DrawPixel(X + cy, Y - cx, CircleColor);
      DrawPixel(X - cy, Y + cx, CircleColor);
      DrawPixel(X - cy, Y - cx, CircleColor);

      int e0 = error + 1 + (cx << 1);
      int e1 = error + 1 + (cy << 1);

      if (-e0 < e1)
      {
         error = e0;
         cx++;
      }
      else
      {
         error = e1;
         cy++;
      }
   }
}

int main()
{
   InitWindow(640, 480, "Basic Window");

   SetTargetFPS(60);

   while (!WindowShouldClose())
   {
      BeginDrawing();

         ClearBackground(RAYWHITE);
         DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

         DrawCircle(50, 50, 25, RED);
         DrawCircleLines(110, 50, 25.0f, RED); //raylib

      EndDrawing();
   }

   CloseWindow();
}
