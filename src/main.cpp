#include <raylib.h>
#include "raymath.h"
#include <iostream>
#include <vector>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"
#define ZEZULA (Color){ 1,0, 1, 0 }
#define MIKU (Color){25, 224, 216,255}
#define MIKU_SKIN (Color){253, 245, 230,255}
#define MIKU_EYES (Color){8, 109, 209,255}

class Object
{
    public:
    Color barv;
    int x,y;

    private:
};


int main() 
{
    const Color backgoundColor = GREEN;
    
    int screenWidth = 1500;
    int screenHeight = 1000;



    InitWindow(GetScreenWidth(), GetScreenHeight(), "Vesnice");// vytvoreni okna

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    

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

    // počet buněk nebo kostek
    const int cells = gridArea * 2 / GRID_SIZE;

    bool run = false;
    bool pause = false;
    bool settings = false;

    float scale;

    int mousehold = 0;
    
    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    
    for (int x = 0; x < cells; x++) {
        for (int y = 0; y < cells; y++) {
            grid[x][y].barv = ZEZULA;
            grid[x][y].x = x;
            grid[x][y].y = y;
        }
    }
    grid[1][1].barv = RED;
    grid[1][cells-1].barv = RED;
    grid[cells/2][cells/2].barv = RED;

    SetExitKey(0);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(YELLOW);
        // start menu
        if (!run) {
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, startButtonY + 100, buttonWidth, buttonHeight }, "Start Game") && !settings) {
                run = true;
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, quitButtonY+300, buttonWidth, buttonHeight }, "Quit") && !settings) {
                EndDrawing();
                CloseWindow();
                return 0;
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, LoadButtonY + 100, buttonWidth, buttonHeight }, "Load game") && !settings) {
                // nic zatim nedela
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, SettingsButtonY + 100, buttonWidth, buttonHeight }, "Settings") && !settings) {
                ClearBackground(YELLOW);
                settings = true;
            }
        }

        if(settings) {
            DrawRectangle(screenWidth / 2 - screenWidth/4, screenHeight / 2 - screenHeight/4 -100, screenWidth/2, screenHeight/2 + 200, Color(BROWN));
            if(GuiButton((Rectangle){ screenWidth / 2 - screenWidth/4, screenHeight /2 - screenHeight/4 -100, 50, 50 }, "<")) {
                settings = false;
            }
        }

        if(run) {

            if(IsKeyPressed(KEY_ESCAPE)) {
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

            for (int i = 0; i < cells; i++) {
                for (int j = 0; j < cells; j++) {
                    
                    int drawX = (i * GRID_SIZE) - gridArea; 
                    int drawY = (j * GRID_SIZE) - gridArea;
                    if(grid[i][j].barv.r != ZEZULA.r || grid[i][j].barv.g != ZEZULA.g || grid[i][j].barv.b != ZEZULA.b || grid[i][j].barv.a != ZEZULA.a)
                    DrawRectangle(drawX, drawY, GRID_SIZE, GRID_SIZE, grid[i][j].barv);
                }
            }
            
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !pause)
            {
                switch (mousehold)
                {
                case 1:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = MIKU;
                    break;
                case 2:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = MIKU_SKIN;
                    break;
                case 3:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = MIKU_EYES;
                    break;
                case 4:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = (Color){209, 8, 125,255};
                    break;
                case 5:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = BLACK;
                    break;
                case 67:
                        grid[(snapX + gridArea) / GRID_SIZE][(snapY + gridArea) / GRID_SIZE].barv = ZEZULA;
                    break;
                default:
                    break;
                }                
            }
            

            

            EndMode2D();
            //in game menu
            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));

            float fromtop = 50;
            if (GuiButton((Rectangle){0, fromtop, 100, 50}, " Miku color")){

                mousehold = 1;
            }
            if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, " Face color")){

                mousehold = 2;
            }
            if (GuiButton((Rectangle){0, fromtop+100, 100, 50}, " Eye color")){

                mousehold = 3;
            }
            if (GuiButton((Rectangle){0, fromtop+150, 100, 50}, " out of ideas color")){

                mousehold = 4;
            }
            if (GuiButton((Rectangle){0, fromtop+150, 100, 50}, " Blackzula color")){

                mousehold = 5;
            }
            if (GuiButton((Rectangle){0, fromtop+200, 100, 50}, " DELETE")){

                mousehold = 67;
            }



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