#include <iostream>
#include <algorithm>
#include "raylib.h"
#include "rlgl.h"
#include "random.h"

//TYPES, STRUCTS, AND FUNCTIONS
typedef struct {

    Vector2 position;    
    float radius;
    float angleDeg;

} Projectile;

typedef struct {

    Vector2 position;
    Vector2 size;

} Paddle;

typedef struct {
    Vector2 position;
    std::string text;
    int fontSize;
} Label;


typedef enum class GameState {
    WAIT_TO_START = 0,
    IN_PROGRESS
} GameState;

Vector2 operator+(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2{ lhs.x + rhs.x, lhs.y + rhs.y };
}

Vector2 operator-(const Vector2& lhs, const Vector2& rhs)
{
    return Vector2{ lhs.x - rhs.x, lhs.y - rhs.y };
}


// Program main entry point
int main(void)
{
    // Initialization
    
    std::cout << "Program Starting!" << '\n';

    constexpr int screenWidth{ 800 };
    constexpr int screenHeight{ 450 };
    constexpr Vector2 screenCenter = Vector2{ screenWidth / 2, screenHeight / 2 };
    
    InitWindow(screenWidth, screenHeight, "my first raylib!");

    Projectile projectile {screenCenter, 5.0f, 12.0f };
    Paddle playerPaddle { screenCenter + Vector2{340.0f, 0.0f}, Vector2{15.0f, 50.0f} };
    Paddle enemyPaddle{ screenCenter + Vector2{-340.0f, 0.0f}, Vector2{15.0f, 50.0f} };
    Label playerScoreLabel { screenCenter + Vector2{200.0f, -200.0f}, "0", 35};
    Label enemyScoreLabel{ screenCenter + Vector2{-200.0f, -200.0f}, "0", 35 };
    constexpr float paddleSpeed{ 550.0f };
    constexpr float projectileSpeed{700.0f};
    constexpr int bounceVariation{ 10 };
    //referring to how many pixels the projectile needs to be teleported away from the wall/paddle
    //just so funky collision bs doesn't happen
    constexpr int bounceAdjustment{ 8 };
    SetTargetFPS(60);

    int playerScore{ 0 };
    int enemyScore{ 0 };
    GameState gameState{ GameState::WAIT_TO_START };


    
    while (!WindowShouldClose())
    {        

        float delta = GetFrameTime();

        //UPDATE
        switch (gameState)
        {
        case GameState::WAIT_TO_START:

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN))
            {
                gameState = GameState::IN_PROGRESS;
            }
            break;

        case GameState::IN_PROGRESS:
            
            if (IsKeyDown(KEY_UP))
            {
                playerPaddle.position.y -= paddleSpeed * delta;

            }
            else if (IsKeyDown(KEY_DOWN))
            {
                playerPaddle.position.y += paddleSpeed * delta;

             }


            if (playerPaddle.position.y < 0)
            {
                playerPaddle.position.y = 0;
            }
            else if (playerPaddle.position.y > screenHeight - playerPaddle.size.y)
            {
                playerPaddle.position.y = screenHeight - playerPaddle.size.y;
            }

            float adjustedEnemyPaddlePosition = enemyPaddle.position.y + enemyPaddle.size.y / 2;
            float toleranceAreaMin = adjustedEnemyPaddlePosition - 10;
            float toleranceAreaMax = adjustedEnemyPaddlePosition + 10;

            if (projectile.position.y > toleranceAreaMax)
            {
                enemyPaddle.position.y += paddleSpeed * delta;
            }
            else if (projectile.position.y < toleranceAreaMin)
            {
                enemyPaddle.position.y -= paddleSpeed * delta;
            }

            if (enemyPaddle.position.y < 0)
            {
                enemyPaddle.position.y = 0;
            }
            else if (enemyPaddle.position.y > screenHeight - enemyPaddle.size.y)
            {
                enemyPaddle.position.y = screenHeight - enemyPaddle.size.y;
            }

            //projectile code and bounce

            projectile.position.x += projectileSpeed * delta * cos(DEG2RAD * projectile.angleDeg);
            projectile.position.y += projectileSpeed * delta * sin(DEG2RAD * projectile.angleDeg);

            if (projectile.position.x >= screenWidth)
            {
                projectile.position = screenCenter;
                projectile.angleDeg += 180.0f;
                enemyScore += 1;
                gameState = GameState::WAIT_TO_START;
            }

            if (projectile.position.x < 0)
            {
                projectile.position = screenCenter;
                projectile.angleDeg += 180.0f;
                playerScore += 1;
                gameState = GameState::WAIT_TO_START;
            }
            if (projectile.position.y >= screenHeight)
            {
                projectile.position.y -= bounceAdjustment;
                projectile.angleDeg = -projectile.angleDeg;
                projectile.angleDeg += Random::get(-bounceVariation, bounceVariation);
            }
            else if (projectile.position.y < 0)
            {
                projectile.position.y += bounceAdjustment;
                projectile.angleDeg = -projectile.angleDeg;
                projectile.angleDeg += Random::get(-bounceVariation, bounceVariation);
            }

            projectile.angleDeg = std::fmodf(projectile.angleDeg, 360.0f);

            //COLLISION

            if (
                CheckCollisionCircleRec(projectile.position, projectile.radius,
                    Rectangle{ playerPaddle.position.x, playerPaddle.position.y, playerPaddle.size.x, playerPaddle.size.y })
                )
            {
                projectile.position.x -= bounceAdjustment;
                projectile.angleDeg = -projectile.angleDeg + 180;
                projectile.angleDeg += Random::get(-bounceVariation, bounceVariation);
            }

            if (
                CheckCollisionCircleRec(projectile.position, projectile.radius,
                    Rectangle{ enemyPaddle.position.x, enemyPaddle.position.y, enemyPaddle.size.x, enemyPaddle.size.y })
                )
            {
                projectile.position.x += bounceAdjustment;
                projectile.angleDeg = -projectile.angleDeg + 180;
                projectile.angleDeg += Random::get(-bounceVariation, bounceVariation);
                
            }
            playerScoreLabel.text = std::to_string(playerScore);
            enemyScoreLabel.text = std::to_string(enemyScore);
            break;

                     
        }
        
        


        //DRAW

        BeginDrawing();

        ClearBackground(RAYWHITE);
        switch (gameState)
        {
        
            

        case GameState::WAIT_TO_START:
            DrawText("Press UP or DOWN to start!", static_cast<int>(screenCenter.x) - 100, static_cast<int>(screenCenter.y), 30, BLACK);
            [[fallthrough]];
        case GameState::IN_PROGRESS:
            DrawRectangleV(playerPaddle.position, playerPaddle.size, RED);
            DrawRectangleV(enemyPaddle.position, enemyPaddle.size, RED);
            DrawCircleV(projectile.position, projectile.radius, RED);

            DrawText(playerScoreLabel.text.c_str(), static_cast<int>(playerScoreLabel.position.x), static_cast<int>(playerScoreLabel.position.y), playerScoreLabel.fontSize, GRAY);
            DrawText(enemyScoreLabel.text.c_str(), static_cast<int>(enemyScoreLabel.position.x), static_cast<int>(enemyScoreLabel.position.y), enemyScoreLabel.fontSize, GRAY);
        }
       

        
        
        EndDrawing();

    }
    //DE-INITIALIZATION
    CloseWindow();

    return 0;
}
