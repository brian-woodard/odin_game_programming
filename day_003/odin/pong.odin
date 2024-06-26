package main

import "core:fmt"
import rl "vendor:raylib"

main :: proc() {
    paddle := rl.Rectangle{100, 100, 180, 30}
    paddle_speed: f32 = 10

    ball := rl.Rectangle{100, 150, 30, 30}
    ball_dir := rl.Vector2{0, -1}
    ball_speed: f32 = 10

    rl.InitWindow(1280, 720, "Pong")
    rl.SetTargetFPS(60)

    for !rl.WindowShouldClose() {

        if rl.IsKeyDown(rl.KeyboardKey.A) {
            paddle.x -= paddle_speed
        }
        if rl.IsKeyDown(rl.KeyboardKey.D) {
            paddle.x += paddle_speed
        }

        next_ball_rect := ball
        next_ball_rect.y += ball_speed * ball_dir.y

        if next_ball_rect.y >= 720 - ball.height || next_ball_rect.y <= 0 {
           ball_dir.y *= -1
        }

        if rl.CheckCollisionRecs(next_ball_rect, paddle) {
           ball_dir.y *= -1
        }

        ball.y += ball_speed * ball_dir.y

        rl.ClearBackground(rl.BLACK)

        rl.BeginDrawing()

        rl.DrawRectangleRec(paddle, rl.WHITE)
        rl.DrawRectangleRec(ball, rl.RED)

        rl.EndDrawing()
    }
}
