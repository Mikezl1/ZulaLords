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


        
    }
    
    CloseWindow();
}