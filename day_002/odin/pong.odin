package main

import "core:fmt"
import rl "vendor:raylib"

main :: proc() {
    pos: i32 = 100

    rl.InitWindow(1280, 720, "Pong")
    rl.SetTargetFPS(60)

    for !rl.WindowShouldClose() {

        if rl.IsKeyDown(rl.KeyboardKey.A) {
            pos -= 10
        }
        if rl.IsKeyDown(rl.KeyboardKey.D) {
            pos += 10
        }

        rl.ClearBackground(rl.BLACK)

        rl.BeginDrawing()

        rl.DrawRectangle(pos, 100, 180, 30, rl.WHITE)

        rl.EndDrawing()
    }
}
