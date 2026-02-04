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

#define GRID_SIZE 50 // Velikost ctverecku

class Object
{
    public:
    Color barv;
    int x,y;
    int drawX, drawY;
    void draw();
    private:

};

void Object::draw()
{
    DrawRectangle(drawX, drawY, GRID_SIZE, GRID_SIZE, barv);
    DrawRectangleV({(float)drawX,(float)drawY},{(float)GRID_SIZE-10.0f, (float)GRID_SIZE-0.5f},barv);
    return;
}


Camera2D cameraUpdate(Camera2D camera) {
                //kamera věci
            // Translate based on mouse right click
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector2 delta = GetMouseDelta();
                delta = Vector2Scale(delta, -1.0f/camera.zoom);
                camera.target = Vector2Add(camera.target, delta);
            }

            float wheel = GetMouseWheelMove();
            if ((wheel != 0)) {
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
    // Not implemented in this example
    return camera;
}


Vector2 GetCameraCenterWorld(Camera2D camera) {
    Vector2 center = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
    return GetScreenToWorld2D(center, camera);
}

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
    //const int GRID_SIZE = 50;  přesunulo se jako definice

    // Velikost gridu
    const int gridArea = 50000;

    // počet buněk nebo kostek
    const int cells = gridArea * 2 / GRID_SIZE;

    bool run = false;
    bool pause = false;
    bool settings = false;

    int mousehold = 0;
    
    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    //grid setup
    for (int x = 0; x < cells; x++) {
        for (int y = 0; y < cells; y++) {
            grid[x][y].barv = ZEZULA;
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].drawX = (x * GRID_SIZE) - gridArea; 
            grid[x][y].drawY = (y * GRID_SIZE) - gridArea;

        }
    }
    grid[1][1].barv = RED;
    grid[1][cells-1].barv = RED;
    grid[cells/2][cells/2].barv = RED;

    int StartGridX = 0;
    int StartGridY = 0;

    SetExitKey(0);
    while (!WindowShouldClose())// main loop
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

        //game
        if(run) {

            if(IsKeyPressed(KEY_ESCAPE)) {
                pause = !pause;
            }

            if(!pause)camera = cameraUpdate(camera);

            ClearBackground(backgoundColor);
            BeginMode2D(camera);

            //modre označeni pozice myši
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            int snapX = (int)floorf(mouseWorldPos.x / GRID_SIZE) * GRID_SIZE; // floorf  zaokrouhly dolu a potom to bude správný int pro GRID
            int snapY = (int)floorf(mouseWorldPos.y / GRID_SIZE) * GRID_SIZE;

            DrawRectangle(snapX, snapY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));

            // vykresleni všech buněk ktere jsou vidět na obrazovce
            Vector2 BotRig  = { (float)GetScreenWidth(), (float)GetScreenHeight() };

            Vector2 TopLeft = GetScreenToWorld2D({ 0.0f, 0.0f }, camera);
            Vector2 BotRight  = GetScreenToWorld2D(BotRig, camera);

            Vector2 TopLVec = (TopLeft / GRID_SIZE) * GRID_SIZE;
            Vector2 BotRVec = (BotRight / GRID_SIZE) * GRID_SIZE;

            Vector2 TL_drw = {(TopLVec.x + gridArea) / GRID_SIZE,(TopLVec.y + gridArea) / GRID_SIZE};
            Vector2 BR_drw = {(BotRVec.x + gridArea) / GRID_SIZE,(BotRVec.y + gridArea) / GRID_SIZE};

            //aby se neprintovaly neexistujici bunky
            if ((int)TL_drw.x < 0) TL_drw.x = 0;

            if ((int)BR_drw.x > cells) BR_drw.x = cells;

            if ((int)TL_drw.y < 0) TL_drw.y = 0;
            
            if ((int)BR_drw.y > cells) BR_drw.y = cells;

            for (int i = TL_drw.x; i < BR_drw.x; i++) {
                for (int j = TL_drw.y; j < BR_drw.y; j++) {

                    if(grid[i][j].barv.r != ZEZULA.r || grid[i][j].barv.g != ZEZULA.g || grid[i][j].barv.b != ZEZULA.b || grid[i][j].barv.a != ZEZULA.a)
                    grid[i][j].draw();
                }
            }         

            // Vykreslení gridu
            for (int x = -gridArea; x < gridArea; x += GRID_SIZE) {
                DrawLine(x, -gridArea, x, gridArea, Color{ 50, 50, 80, 255 });
            }

            for (int y = -gridArea; y < gridArea; y += GRID_SIZE) {
                DrawLine(-gridArea, y, gridArea, y, Color{ 50, 50, 80, 255 });
            }
////////////////  TODO dodělat aby to bylo viditelne když se tahne
            int gridX = (snapX + gridArea) / GRID_SIZE;
            int gridY = (snapY + gridArea) / GRID_SIZE;
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !pause)
            {
                StartGridX = gridX;
                StartGridY = gridY;

            }   

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !pause)
            {
                
                if ( abs(gridX - StartGridX) >= abs(gridY - StartGridY))
                {
                    int Start = (gridX < StartGridX) ? gridX : StartGridX;// aby to šlo do minusu
                    int End = (gridX > StartGridX) ? gridX : StartGridX;
                    for (int i = Start; i <= End; i++)
                    {
                        if (i >= 0 && i < cells && StartGridY >= 0 && StartGridY < cells) 
                        {
                            switch (mousehold)
                            {
                            case 1:
                                    grid[i][StartGridY].barv = MIKU;
                                break;
                            case 2:
                                    grid[i][StartGridY].barv = MIKU_SKIN;
                                break;
                            case 3:
                                    grid[i][StartGridY].barv = MIKU_EYES;
                                break;
                            case 4:
                                    grid[i][StartGridY].barv = (Color){209, 8, 125,255};
                                break;
                            case 5:
                                    grid[i][StartGridY].barv = BLACK;
                                break;
                            case 67:
                                    grid[i][StartGridY].barv = ZEZULA;
                                break;
                            default:
                                break;
                            }  
                        }
                    }
                    
                }
                else
                {
                    int Start = (gridY < StartGridY) ? gridY : StartGridY;// aby to šlo do minusu
                    int End = (gridY > StartGridY) ? gridY : StartGridY;
                    for (int i = Start; i <= End; i++)
                    {
                        if (StartGridX >= 0 && StartGridX < cells && i >= 0 && i < cells ) 
                        {
                            switch (mousehold)
                            {
                            case 1:
                                    grid[StartGridX][i].barv = MIKU;
                                break;
                            case 2:
                                    grid[StartGridX][i].barv = MIKU_SKIN;
                                break;
                            case 3:
                                    grid[StartGridX][i].barv = MIKU_EYES;
                                break;
                            case 4:
                                    grid[StartGridX][i].barv = (Color){209, 8, 125,255};
                                break;
                            case 5:
                                    grid[StartGridX][i].barv = BLACK;
                                break;
                            case 67:
                                    grid[StartGridX][i].barv = ZEZULA;
                                break;
                            default:
                                break;
                            }  
                        }
                    }
                    
                }
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !pause)
            {

                int gridX = (snapX + gridArea) / GRID_SIZE;
                int gridY = (snapY + gridArea) / GRID_SIZE;


                //aby se nepsalo mimo bunky
                if (gridX >= 0 && gridX < cells && gridY >= 0 && gridY < cells ) 
                {
                    switch (mousehold)
                    {
                    case 1:
                            grid[gridX][gridY].barv = MIKU;
                        break;
                    case 2:
                            grid[gridX][gridY].barv = MIKU_SKIN;
                        break;
                    case 3:
                            grid[gridX][gridY].barv = MIKU_EYES;
                        break;
                    case 4:
                            grid[gridX][gridY].barv = (Color){209, 8, 125,255};
                        break;
                    case 5:
                            grid[gridX][gridY].barv = BLACK;
                        break;
                    case 67:
                            grid[gridX][gridY].barv = ZEZULA;
                        break;
                    default:
                        break;
                    }  
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
            if (GuiButton((Rectangle){0, fromtop+200, 100, 50}, " Black color")){

                mousehold = 5;
            }
            if (GuiButton((Rectangle){0, fromtop+250, 100, 50}, " DELETE")){

                mousehold = 67;
            }


            //in pause menu
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