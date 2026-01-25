#include <raylib.h>
#include "raymath.h"
#include <iostream>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"


int main() 
{
    const Color backgoundColor = GREEN;
    
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;



    InitWindow(screenWidth, screenHeight, "Vesnice");// vytvoreni okna
    
    SetTargetFPS(60);
  
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    float buttonWidth = 200;
    float buttonHeight = 50;
    float startButtonY = screenHeight / 2 - buttonHeight / 2;
    float quitButtonY = startButtonY + buttonHeight + 10;
    float mainmenuButtonY = startButtonY + 2*(buttonHeight + 10);

    // Velikost ctverecku
    const int GRID_SIZE = 50;

    // Velikost gridu
    const int gridArea = 50000;

    bool run = false;
    bool pause = false;

    float scale;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(YELLOW);

        run = GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, startButtonY, buttonWidth, buttonHeight }, "Start Game");
        
        while(run) {
            //kamera věci
            // Translate based on mouse right click
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 delta = GetMouseDelta();
                delta = Vector2Scale(delta, -1.0f/camera.zoom);
                camera.target = Vector2Add(camera.target, delta);
            }

            float wheel = GetMouseWheelMove();
            if (wheel != 0) {
                // Get the world point that is under the mouse
                Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

                // Set the offset to where the mouse is
                camera.offset = GetMousePosition();

                // Set the target to match, so that the camera maps the world space point 
                // under the cursor to the screen space point under the cursor at any zoom
                camera.target = mouseWorldPos;

                // Zoom increment
                // Uses log scaling to provide consistent zoom speed
                scale = 0.2f*wheel;
                camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            }
            ClearBackground(backgoundColor);
            BeginMode2D(camera);

            if(IsKeyPressed(KEY_A)) {
                pause = true;
                float scale2 = scale;
            }    
                while(pause) {
                    camera.zoom = 1.0f;
                    ClearBackground(YELLOW);
                    if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, startButtonY, buttonWidth, buttonHeight }, "Resume Game")){
                        pause = false;
                        break;
                    }
                    if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, mainmenuButtonY, buttonWidth, buttonHeight }, "To Main Menu")) {
                        pause = false;
                        run = false;
                        break;
                    }
                    if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, quitButtonY, buttonWidth, buttonHeight }, "Quit")) {
                        EndMode2D();
                        EndDrawing();
                        CloseWindow();
                        return 0;
                    }
                    EndDrawing();
                }
            
            // Vykreslení gridu
            for (int x = -gridArea; x < gridArea; x += GRID_SIZE) {
                DrawLine(x, -gridArea, x, gridArea, Color{ 50, 50, 80, 255 });
            }

            for (int y = -gridArea; y < gridArea; y += GRID_SIZE) {
                DrawLine(-gridArea, y, gridArea, y, Color{ 50, 50, 80, 255 });
            }
            EndDrawing();
            EndMode2D();
        }       

        if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, quitButtonY, buttonWidth, buttonHeight }, "Quit")) {
            EndMode2D();
            EndDrawing();
            CloseWindow();
            return 0;
        }   
        
        DrawFPS(10, 10);
        EndMode2D();
        EndDrawing();           
    }
    
    CloseWindow();
    return 0;
}