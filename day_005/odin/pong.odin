package main

import "core:fmt"
import "core:math"
import "core:math/linalg"
import "core:math/rand"
import rl "vendor:raylib"

Game_State :: struct {
    window_size:         rl.Vector2,
    paddle:              rl.Rectangle,
    ai_paddle:           rl.Rectangle,
    ai_target_y:         f32,
    ai_reaction_delay:   f32,
    ai_reaction_timer:   f32,
    paddle_speed:        f32,
    ball:                rl.Rectangle,
    ball_dir:            rl.Vector2,
    ball_speed:          f32, 
    score_player:        int,
    score_cpu:           int,
    boost_timer:         f32,
    boost_time:          f32,
}

reset :: proc(using gs: ^Game_State) {
    paddle_margin: f32 = 50
    angle := rand.float32_range(-45, 46)
    if rand.int_max(100) % 2 == 0 do angle += 180
    r := math.to_radians(angle)

    ball_dir.x = math.cos(r)
    ball_dir.y = math.sin(r)

    ball.x = window_size.x / 2 - ball.width / 2
    ball.y = window_size.y / 2 - ball.height / 2

    paddle.x = window_size.x - (paddle.width + paddle_margin)
    paddle.y = window_size.y / 2 - paddle.height / 2

    ai_paddle.x = paddle_margin
    ai_paddle.y = window_size.y / 2 - paddle.height / 2
}

ball_dir_calculate :: proc(ball: rl.Rectangle, paddle: rl.Rectangle) -> (rl.Vector2, bool) {
    if rl.CheckCollisionRecs(ball, paddle) {
        ball_center := rl.Vector2 { ball.x + ball.width / 2, ball.y + ball.height / 2 }
        paddle_center := rl.Vector2 { paddle.x + paddle.width / 2, paddle.y + paddle.height / 2 }
        return linalg.normalize0(ball_center - paddle_center), true
    }
    return {}, false
}

main :: proc() {
    gs := Game_State {
        window_size = {1280, 720},
        paddle = {width = 30, height = 80},
        ai_paddle = {width = 30, height = 80},
        ai_reaction_delay = 0.1,
        paddle_speed = 10,
        ball = {width = 30, height = 30},
        ball_speed = 10,
        boost_time = 1.0
    }
    reset(&gs)

    using gs

    rl.InitWindow(i32(window_size.x), i32(window_size.y), "Pong")
    rl.SetTargetFPS(60)

    rl.InitAudioDevice()
    defer rl.CloseAudioDevice()

    sfx_hit := rl.LoadSound("hit.wav")
    sfx_win := rl.LoadSound("win.wav")
    sfx_lose := rl.LoadSound("lose.wav")

    for !rl.WindowShouldClose() {
        delta := rl.GetFrameTime()

        boost_timer -= delta

        if rl.IsKeyDown(.UP) {
            paddle.y -= paddle_speed
        }
        if rl.IsKeyDown(.DOWN) {
            paddle.y += paddle_speed
        }
        if rl.IsKeyPressed(.SPACE) {
            if boost_timer < 0 {
                boost_timer = boost_time  
                rl.PlaySound(sfx_lose)
            }
        }

        paddle.y = linalg.clamp(paddle.y, 0, window_size.y - paddle.height)

        // AI movement
        // increase timer by time between last frame and this one
        ai_reaction_timer += delta
        // if the timer is done
        if ai_reaction_timer >= ai_reaction_delay {
            // reset the timer
            ai_reaction_timer = 0
            // use ball from last frame for extra delay
            ball_mid := ball.y + ball.height / 2
            // if the ball is heading left
            if ball_dir.x < 0 {
                // set the target to the ball
                ai_target_y = ball_mid - ai_paddle.height / 2
                // add or subtract 0-20 to add inaccuracy
                ai_target_y += rand.float32_range(-20, 20)
            } else {
                // set the target to screen middle
                ai_target_y = window_size.y / 2 - ai_paddle.height / 2 
            }
        }

        // calculate the distance between the paddle and target
        ai_paddle_mid := ai_paddle.y + ai_paddle.height / 2
        target_diff := ai_target_y - ai_paddle.y
        // move either paddle_speed distance or less
        // won't bounce around so much
        ai_paddle.y += linalg.clamp(target_diff, -paddle_speed, paddle_speed) * 0.65
        // clamp to window_size
        ai_paddle.y = linalg.clamp(ai_paddle.y, 0, window_size.y - ai_paddle.height)

        next_ball_rect := ball
        next_ball_rect.x += ball_speed * ball_dir.x
        next_ball_rect.y += ball_speed * ball_dir.y

        if next_ball_rect.x >= window_size.x - ball.width {
            score_cpu += 1
            rl.PlaySound(sfx_lose)
            reset(&gs)
        }

        if next_ball_rect.x <= 0 {
            score_player += 1
            rl.PlaySound(sfx_win)
            reset(&gs)
        }

        if next_ball_rect.y >= window_size.y - ball.height || next_ball_rect.y <= 0 {
            ball_dir.y *= -1
            rl.PlaySound(sfx_hit)
        }

        if new_dir, ok := ball_dir_calculate(next_ball_rect, paddle); ok {
            // if the button was pressed in the last boost_time seconds
            if boost_timer > 0 {
                // boost_timer / boost_time will give us a percentage (let's say 30%)
                // we add 1 because we want to increase the speed (130%)
                d := 1 + boost_timer / boost_time
                new_dir *= d
            }
            ball_dir = new_dir
            rl.PlaySound(sfx_hit)
        } else if new_dir, ok := ball_dir_calculate(next_ball_rect, ai_paddle); ok {
            ball_dir = new_dir
            rl.PlaySound(sfx_hit)
        }

        ball.x += ball_speed * ball_dir.x
        ball.y += ball_speed * ball_dir.y

        rl.ClearBackground(rl.BLACK)

        rl.BeginDrawing()

        if boost_timer > 0 {
            rl.DrawRectangleRec(paddle, {u8(255 * ((boost_time - boost_timer) / boost_time)), 255, 255, 255})
        } else {
            rl.DrawRectangleRec(paddle, rl.WHITE)
        }
        rl.DrawRectangleRec(ai_paddle, rl.WHITE)
        rl.DrawRectangleRec(ball, {255, u8(255 - 255 / linalg.length(ball_dir)), 0, 255})

        rl.DrawText(fmt.ctprintf("{}", score_cpu), 12, 12, 32, rl.WHITE)
        rl.DrawText(fmt.ctprintf("{}", score_player), i32(window_size.x) - 28, 12, 32,rl.WHITE)

        rl.EndDrawing()
        free_all(context.temp_allocator)
    }
}
