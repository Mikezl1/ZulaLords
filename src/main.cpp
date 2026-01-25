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
    float startButtonY = screenHeight / 2 - buttonHeight - 100;
    float quitButtonY = startButtonY + buttonHeight +10;
    float mainmenuButtonY = startButtonY + buttonHeight + 10;
    float SettingsButtonY = startButtonY + buttonHeight + 10;
    float LoadButtonY = SettingsButtonY + buttonHeight + 10;

    // Velikost ctverecku
    const int GRID_SIZE = 50;

    // Velikost gridu
    const int gridArea = 50000;

    bool run = false;
    bool pause = false;
    bool settings = false;

    float scale;


    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(YELLOW);

        if (!run) {
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, startButtonY + 100, buttonWidth, buttonHeight }, "Start Game")) {
                run = true;
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, quitButtonY+300, buttonWidth, buttonHeight }, "Quit")) {
                EndMode2D();
                EndDrawing();
                CloseWindow();
                return 0;
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, LoadButtonY + 100, buttonWidth, buttonHeight }, "Load game")) {
                // nic zatim nedela
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, SettingsButtonY + 100, buttonWidth, buttonHeight }, "Settings")) {
                settings = true;
            }
        }

        if(settings) {
            DrawRectangle(screenWidth / 2 - 350, screenHeight / 2 - 400, 700, 800, Color(BROWN));
            if(GuiButton((Rectangle){ screenWidth / 2 - 350, SettingsButtonY - 310, 50, 50 }, "<")) {
                settings = false;
            }
        }

        if(run) {

            if(IsKeyPressed(KEY_A)) {
                pause = !pause;
            }

            //kamera věci
            // Translate based on mouse right click
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !pause) {
                Vector2 delta = GetMouseDelta();
                delta = Vector2Scale(delta, -1.0f/camera.zoom);
                camera.target = Vector2Add(camera.target, delta);
            }

            float wheel = GetMouseWheelMove();
            if ((wheel != 0) && !pause) {
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

            // Vykreslení gridu
            for (int x = -gridArea; x < gridArea; x += GRID_SIZE) {
                DrawLine(x, -gridArea, x, gridArea, Color{ 50, 50, 80, 255 });
            }

            for (int y = -gridArea; y < gridArea; y += GRID_SIZE) {
                DrawLine(-gridArea, y, gridArea, y, Color{ 50, 50, 80, 255 });
            }

            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            int snapX = (int)(mouseWorldPos.x / GRID_SIZE) * GRID_SIZE;
            int snapY = (int)(mouseWorldPos.y / GRID_SIZE) * GRID_SIZE;

            DrawRectangle(snapX, snapY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));

            EndMode2D();

            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));
    
            if(pause) {
                DrawRectangle(screenWidth / 2 - 150, screenHeight/2 - 200, 300, 400, Color{BROWN});
                if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth /2, startButtonY, buttonWidth, buttonHeight}, "Resume Game")){
                    pause = false;
                }
                if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth /2, LoadButtonY - 60, buttonWidth, buttonHeight }, "Load game")) {
                }
                if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth /2, mainmenuButtonY+60, buttonWidth, buttonHeight }, "To Main Menu")) {
                    pause = false;
                    run = false;
                }
                if (GuiButton((Rectangle){ screenWidth / 2 - buttonWidth /2, quitButtonY+200, buttonWidth, buttonHeight }, "Quit")) {
                    EndDrawing();
                    CloseWindow();
                    return 0;
                }
            }
        }       
        DrawFPS(10, 10);
        EndDrawing();           
    }
    
    CloseWindow();
    return 0;
}