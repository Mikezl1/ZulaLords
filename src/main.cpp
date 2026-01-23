#include <raylib.h>
#include "raymath.h"
#include <iostream>


int main() 
{
    const Color backgoundColor = {0, 0, 0, 255};
    
    constexpr int screenWidth = 1920;
    constexpr int screenHeight = 1080;



    InitWindow(screenWidth, screenHeight, "Vesnice");// vytvoreni okna
    
    SetTargetFPS(60);
  
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    
    while (!WindowShouldClose())
    {

        
        //kamera věci
        // Translate based on mouse right click
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            // Set the offset to where the mouse is
            camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point 
            // under the cursor to the screen space point under the cursor at any zoom
            camera.target = mouseWorldPos;

            // Zoom increment
            // Uses log scaling to provide consistent zoom speed
            float scale = 0.2f*wheel;
            camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
        }

        /// zde se počitá
        int MENU_WIDTH = 300;
        const int GRID_SIZE = 50;
        const int gridRadius = 50000;

        BeginDrawing();
            ClearBackground(GREEN);
            BeginMode2D(camera);
            
            // Vykreslení gridu
            for (int x = MENU_WIDTH - gridRadius; x < MENU_WIDTH + gridRadius; x += GRID_SIZE){
            DrawLine(x, -gridRadius, x, gridRadius, Color{ 50, 50, 80, 255 });
            }

            for (int y = -gridRadius; y < gridRadius; y += GRID_SIZE) {
            DrawLine(MENU_WIDTH - gridRadius, y, MENU_WIDTH + gridRadius, y, Color{ 50, 50, 80, 255 });
            }
            

            
            
            
            EndMode2D();

            //DrawFPS(10, 10);
        EndDrawing();
        
    }
    
    CloseWindow();
}