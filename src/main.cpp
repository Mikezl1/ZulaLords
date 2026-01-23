#include <raylib.h>
#include "raymath.h"
#include <iostream>


int main() 
{
    const Color backgoundColor = {0, 0, 0, 255};
    
    constexpr int screenWidth = 1900;
    constexpr int screenHeight = 1000;



    InitWindow(screenWidth, screenHeight, "Vesnice");// vytvoreni okna
    
    SetTargetFPS(60);
  
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    
    int mik = 10;
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
        mik = (mik+1)%100;


        BeginDrawing();
            ClearBackground(backgoundColor);
            BeginMode2D(camera);
            /// zde se maluje
            DrawCircle(screenWidth/2,screenHeight/2,mik,BLUE);
            EndMode2D();

            DrawFPS(10, 10);
        EndDrawing();
    }
    
    CloseWindow();
}