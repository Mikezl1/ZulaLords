#include <raylib.h>
#include "raymath.h"
#include <iostream>
#include <vector>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"
#define GRID_SIZE 50 // Velikost ctverecku
#define gridArea  50000// Velikost gridu
#define cells gridArea * 2 / GRID_SIZE// počet buněk nebo kostek

typedef enum {
    TERRAIN_BLANK,
    TERRAIN_DIRT_PATH,
    TERRAIN_STONE_PATH,
    TERRAIN_LIMESTONE_WALL,
    TERRAIN_COUNT //last one
} TerrainType;

//postnoupnot MUSÍ BÝT STEJNÁ jako v enum
const Color TerrainColors[] = {
    [TERRAIN_BLANK] = {0, 0, 0, 0},
    [TERRAIN_DIRT_PATH] = {139, 69, 19, 255},
    [TERRAIN_STONE_PATH]  = {136,140,141, 255}, 
    [TERRAIN_LIMESTONE_WALL] = {217, 211, 199, 255}
};

struct BuildingTemplate
{
    std::string name;
    int gridWidth;
    int gridHeight;
    Color color;
    void draw(int drawX,int drawY);
};

struct ShopTemplate
{
    std::string name;
    int gridWidth;
    int gridHeight;
    Color color;
    void draw(int drawX,int drawY);
};

void BuildingTemplate::draw(int drawX,int drawY)
{
    DrawRectangle(drawX, drawY, gridWidth * GRID_SIZE, gridHeight * GRID_SIZE, color);
    return;
}

void ShopTemplate::draw(int drawX,int drawY)
{
    DrawRectangle(drawX, drawY, gridWidth * GRID_SIZE, gridHeight * GRID_SIZE, color);
    return;
}

bool operator!=(const Color& a, const Color& b)
{
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

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
    if(barv != TerrainColors[TERRAIN_BLANK])
    DrawRectangle(drawX, drawY, GRID_SIZE, GRID_SIZE, barv);
    
    return;
}


Camera2D cameraUpdate(Camera2D camera) {//kamera věci                
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    float wheel = GetMouseWheelMove();
    if ((wheel != 0)) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        float scale = 0.2f*wheel;
        camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
    }
    return camera;
}

void PrintAll(std::vector<std::vector<Object>>& grid, Camera2D camera)
{
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
            grid[i][j].draw();
        }
    }    
    return;
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

    //InitWindow(1000, 500, "Vesnice");// vytvoreni okna

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
    

    SetTargetFPS(60);
  
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    float settingsboxX = screenWidth / 4;
    float settingsboxY = screenHeight / 4 - 100;
    float settingsboxheight = screenHeight / 2 + 200;
    float settingsboxwidth = screenWidth / 2;

    float pausemenuH = 400;
    float pausemenuW = 300;
    float pausemenuX = screenWidth / 2 - pausemenuW/2;
    float pausemenuY = screenHeight / 2 - pausemenuH/2;
    float pausemenuspacing = 10;

    float buttonWidth = 200;
    float buttonHeight = 50;
    float startButtonY = screenHeight / 2 - buttonHeight - 100;
    float quitButtonY = startButtonY + buttonHeight +10;
    float SettingsButtonY = startButtonY + buttonHeight + 10;
    float LoadButtonY = SettingsButtonY + buttonHeight + 10;

    bool run = false;
    bool pause = false;
    bool settings = false;
    bool build = 0;
    bool paths = 0;
    bool houses = 0;
    bool destroy = 0;
    bool shops = 0;

    int mousehold = 0;

    int money = 50;

    //building stuff
    bool isdragg = 0;
    BuildingTemplate draggedTemplate;
    std::vector<BuildingTemplate> BuildingTemplate = {
        {"Basic House", 10, 5, GRAY,},
        {"Basic House2", 6, 7, BROWN,},
        
    };

    ShopTemplate draggedTemplateShop;
    std::vector<ShopTemplate> ShopTemplate = {
        {"Basic Shop", 2, 3, GRAY,},
        {"Basic Shop2", 6, 7, BROWN,},
    };

    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    //grid setup
    for (int x = 0; x < cells; x++) {
        for (int y = 0; y < cells; y++) {
            grid[x][y].barv = TerrainColors[TERRAIN_BLANK];
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
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, LoadButtonY + 100, buttonWidth, buttonHeight }, "Load game") && !settings) {
                // nic zatim nedela
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, SettingsButtonY + 100, buttonWidth, buttonHeight }, "Settings") && !settings) {
                ClearBackground(YELLOW);
                settings = true;
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, quitButtonY+300, buttonWidth, buttonHeight }, "Quit") && !settings) {
                EndDrawing();
                CloseWindow();
                return 0;
            }
        }

        if(settings) {
            DrawRectangle(settingsboxX, settingsboxY, settingsboxwidth, settingsboxheight, Color(BROWN));
            if(GuiButton((Rectangle){ settingsboxX, settingsboxY, 50, 50 }, "<")) {
                settings = false; 
            }
        }

        //game
        if(run) {

            if(IsKeyPressed(KEY_ESCAPE) && !build) {
                pause = !pause;
            }

            if(!pause)camera = cameraUpdate(camera);

            ClearBackground(backgoundColor);
            BeginMode2D(camera);

            //modre označeni pozice myši
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            int snapX = (int)floorf(mouseWorldPos.x / GRID_SIZE) * GRID_SIZE; // floorf  zaokrouhly dolu a potom to bude správný int pro GRID
            int snapY = (int)floorf(mouseWorldPos.y / GRID_SIZE) * GRID_SIZE;
            
            if(!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) DrawRectangle(snapX, snapY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));

            // Vykreslení gridu
            for (int x = -gridArea; x < gridArea; x += GRID_SIZE) {
                DrawLine(x, -gridArea, x, gridArea, Color{ 50, 50, 80, 55 });
            }

            for (int y = -gridArea; y < gridArea; y += GRID_SIZE) {
                DrawLine(-gridArea, y, gridArea, y, Color{ 50, 50, 80, 55 });
            }

            PrintAll(grid,camera);// vykresleni všech buněk ktere jsou vidět na obrazovce

            //line spawn veci
            int gridX = (snapX + gridArea) / GRID_SIZE;
            int gridY = (snapY + gridArea) / GRID_SIZE;
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !pause)
            {
                StartGridX = gridX;
                StartGridY = gridY;
            }   

            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !pause)
            {
                int StartX = (gridX < StartGridX) ? gridX : StartGridX;
                int EndX = (gridX > StartGridX) ? gridX : StartGridX;
                int StartY = (gridY < StartGridY) ? gridY : StartGridY;
                int EndY = (gridY > StartGridY) ? gridY : StartGridY;

                for (int i = StartX; i <= EndX; i++)
                {
                    for (int ii = StartY; ii <= EndY; ii++)
                    {
                        if (i >= 0 && i < cells && ii >= 0 && ii < cells ) 
                        {
                            int drawX = grid[i][ii].drawX;
                            int drawY = grid[i][ii].drawY;
                            DrawRectangle(drawX, drawY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));
                        }
                    }
                }    
            }

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !pause && (paths | destroy) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}))
            {
                    int StartX = (gridX < StartGridX) ? gridX : StartGridX;// aby to šlo do minusu
                    int EndX = (gridX > StartGridX) ? gridX : StartGridX;
                    int StartY = (gridY < StartGridY) ? gridY : StartGridY;// aby to šlo do minusu
                    int EndY = (gridY > StartGridY) ? gridY : StartGridY;
                    
                    for (int i = StartX; i <= EndX; i++)
                    {
                        for (int ii = StartY; ii <= EndY; ii++)
                        {
                            if (i >= 0 && i < cells && ii >= 0 && ii < cells ) 
                            {
                                grid[i][ii].barv = TerrainColors[mousehold] ;
                            }
                        }
                    }

                //}
            }

            // normal spawn věci
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !pause && (paths | destroy))
            {

                int gridX = (snapX + gridArea) / GRID_SIZE;
                int gridY = (snapY + gridArea) / GRID_SIZE;


                //aby se nepsalo mimo bunky
                if (gridX >= 0 && gridX < cells && gridY >= 0 && gridY < cells ) 
                {
                    grid[gridX][gridY].barv = TerrainColors[mousehold] ;
                }
              
            }


            if(houses && isdragg) {//když dům
                draggedTemplate.draw(snapX,snapY);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !pause)
                {// tohle se musi vymyslet nejak jinak ale pro představu je to zatim takhle

                    int HouseGridX = (snapX + gridArea) / GRID_SIZE;
                    int HouseGridY = (snapY + gridArea) / GRID_SIZE;

                    for (int x = 0; x < draggedTemplate.gridWidth; x++)
                    {
                        for (int y = 0; y < draggedTemplate.gridHeight; y++)
                        {
                            int targetX = HouseGridX + x;
                            int targetY = HouseGridY + y;

                            if (targetX >= 0 && targetX < cells && targetY >= 0 && targetY < cells) 
                            {
                                grid[targetX][targetY].barv = draggedTemplate.color; 
                            }
                        }
                    }
                    isdragg = 0;
                }                
            }

            if(shops && isdragg) {//když shop
                draggedTemplateShop.draw(snapX,snapY);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !pause)
                {// tohle se musi vymyslet nejak jinak ale pro představu je to zatim takhle

                    int ShopGridX = (snapX + gridArea) / GRID_SIZE;
                    int ShopGridY = (snapY + gridArea) / GRID_SIZE;

                    for (int x = 0; x < draggedTemplateShop.gridWidth; x++)
                    {
                        for (int y = 0; y < draggedTemplateShop.gridHeight; y++)
                        {
                            int targetX = ShopGridX + x;
                            int targetY = ShopGridY + y;

                            if (targetX >= 0 && targetX < cells && targetY >= 0 && targetY < cells) 
                            {
                                grid[targetX][targetY].barv = draggedTemplateShop.color; 
                            }
                        }
                    }
                    isdragg = 0;
                }                
            }

            EndMode2D();

            //in game menu
            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));
            DrawRectangle(0, 0, screenWidth, 50, Color(BROWN));

            float fromtop = 50;


            GuiValueBox((Rectangle){500, 0, 100, 50}, "Money", &money, 0, 100, false);

            if ((money >= 0)&&(money < 100)) {
                money++;
            }
            else {
                money = 0;
            }

            if (GuiButton((Rectangle){0, fromtop, 100, 50}, "PATHS") && !build){
                paths = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, "HOUSES") && !build){
                houses = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+100, 100, 50}, "SHOPS") && !build){
                build = true;
                shops = true;
            }

            if (GuiButton((Rectangle){0, fromtop+150, 100, 50}, "DESTROY") && !build){
                destroy = true;
                build = true;
            }

            if(build) {
                DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));
                if(paths) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "DIRT PATH")){
                        mousehold = TERRAIN_DIRT_PATH;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "STONE PATH")){
                        mousehold = TERRAIN_STONE_PATH;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "CANCEL")){
                        paths = false;
                        build = false;
                    }
                }

                if(houses) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "HOUSE 1")){
                        draggedTemplate = BuildingTemplate[0];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "HOUSE 2")){
                        draggedTemplate = BuildingTemplate[1];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "CANCEL")){
                        houses = false;
                        build = false;
                    }
                }

                if (shops) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "SHOP 1")){
                        draggedTemplateShop = ShopTemplate[0];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "CANCEL")){
                        shops = false;
                        build = false;
                    }
                }

                if(destroy) {
                    mousehold = TERRAIN_BLANK;
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "CANCEL")){
                        destroy = false;
                        build = false;
                    }
                }


                if (IsKeyPressed(KEY_ESCAPE))
                {
                    paths = false;
                    houses = false;
                    destroy = false;
                    build = false;
                }
            }
            

            //in pause menu
            if(pause) {
                DrawRectangle(pausemenuX, pausemenuY, pausemenuW, pausemenuH, Color{BROWN});
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + buttonHeight, buttonWidth, buttonHeight}, "Resume Game")){
                    pause = false;
                }
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 2*buttonHeight + pausemenuspacing, buttonWidth, buttonHeight }, "Load game")) {
                }
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 3*buttonHeight + 2*pausemenuspacing, buttonWidth, buttonHeight }, "To Main Menu")) {
                    pause = false;
                    run = false;
                }
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + pausemenuH - 2*buttonHeight, buttonWidth, buttonHeight }, "Quit")) {
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