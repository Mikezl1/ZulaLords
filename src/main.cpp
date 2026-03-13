#include <raylib.h>
#include "raymath.h"
#include <iostream>
#include <vector>
#include <fstream>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#include "constants.h"
#include "npc.h"
#include "buildings.h"

using namespace std;

std::vector<Vector2> activeShops;
std::vector<Vector2> houseslocations;

void gridSetup (std::vector<std::vector<Object>>& grid) {
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

    bool fullscreen = true;
    int screenWidth;
    int screenHeight;

    if(fullscreen) {
        InitWindow(GetScreenWidth(), GetScreenHeight(), "Vesnice");// vytvoreni okna
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
    }
    else {
        screenWidth = 1920;
        screenHeight = 1080;
        InitWindow(screenWidth, screenHeight, "Vesnice");// vytvoreni okna
    }

    

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
    bool build = false;
    bool paths = false;
    bool houses = false;
    bool destroy = false;
    bool shops = false;
    bool no_money = false;
    bool rotate = false;
    

    int mousehold = 0;

    int money = 100;

    int rotatedWidth;
    int rotatedHeight;


    //building stuff
    std::vector<BuildingTemplate> ConstructedBuildings;
    ConstructedBuildings.reserve(100);
    bool isdragg = 0;
    BuildingTemplate draggedTemplate;
    std::vector<BuildingTemplate> BuildingTemplate = {
        {"Basic House", 7, 5, 4, 25, GRAY,},
        {"Basic House2", 7, 9, 6, 50, BROWN,},
        {"Basic Shop", 2, 3, 2, 5, GRAY,},
        {"Basic Shop2", 3, 4, 6, 7, BROWN,},
    };


    BuildingTemplate[0].textura = LoadTexture("dum22.png");
    GenTextureMipmaps(&BuildingTemplate[0].textura);
    SetTextureFilter(BuildingTemplate[0].textura, TEXTURE_FILTER_TRILINEAR); 

    NPC npc1[10000];
    int alive_npc = 0;

    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    //grid setup
    gridSetup(grid);

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
                ifstream myfile ("save.txt");
                if (myfile.is_open()) {
                    // 1. Reset grid to blank first
                    for (int i = 0; i < cells; i++) {
                        for (int j = 0; j < cells; j++) {
                            grid[i][j].barv = TerrainColors[TERRAIN_BLANK];
                        }
                    }

                    // 2. Read only the objects that were saved
                    int x, y, r, g, b, a;
                    while (myfile >> x >> y >> r >> g >> b >> a) {
                        if (x >= 0 && x < cells && y >= 0 && y < cells) {
                            grid[x][y].barv = {(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
                        }
                    }
                    myfile.close();
                    run = true;
                }
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

            if (!pause) {
                for(int i = 0; i < alive_npc+1; i++) {
                    npc1[i].NPC_movement(activeShops, houseslocations, grid);
                }
            }

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

            for(int i = 0; i < alive_npc+1; i++) {
                npc1[i].draw();
            }

            for (auto& budova : ConstructedBuildings) {
                DrawRectangle(budova.drawX, budova.drawY, budova.gridWidth * GRID_SIZE, budova.gridHeight * GRID_SIZE, budova.color);
            }

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
            }

            for(int i = 0; i < alive_npc+1; i++) {
                if ((mouseWorldPos.x <= npc1[i].x + npc1[i].rad && mouseWorldPos.x >= npc1[i].x - npc1[i].rad) && (mouseWorldPos.y <= npc1[i].y + npc1[i].rad && mouseWorldPos.y >= npc1[i].y - npc1[i].rad) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && npc1[i].clicked != true)
                {
                    npc1[i].clicked = true;
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && npc1[i].clicked == true){
                    npc1[i].clicked = false;
                }
            }    

            for (int i =0; i < alive_npc+1; i++)
            {
                if (npc1[i].clicked) {

                    camera.target = {(float)npc1[i].x, (float)npc1[i].y};

                    char age[100];
                    DrawRectangle(npc1[i].x - 50, npc1[i].y - 100, 100, 50, Color{BROWN});
                    DrawText(npc1[i].name, npc1[i].x - 50, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].age);
                    DrawText(age, npc1[i].x + 25, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].doing);
                    DrawText(age, npc1[i].x + 50, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].destinationX);
                    DrawText(age, npc1[i].x + 75, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].destinationY);
                    DrawText(age, npc1[i].x + 125, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].x);
                    DrawText(age, npc1[i].x + 175, npc1[i].y - 100, 20, WHITE);
                    sprintf(age, "%d", npc1[i].y);
                    DrawText(age, npc1[i].x + 225, npc1[i].y - 50, 20, WHITE);
                    sprintf(age, "%d", npc1[i].startX);
                    DrawText(age, npc1[i].x + 275, npc1[i].y - 85, 20, WHITE);
                    sprintf(age, "%d", npc1[i].startY);
                    DrawText(age, npc1[i].x + 325, npc1[i].y - 85, 20, WHITE);
                }
            }

            if((houses || shops) && isdragg) {//když dům
                if (IsKeyPressed(KEY_R) || rotate) {
                    if (IsKeyPressed(KEY_R) && rotate) {
                        rotate = false;
                    }
                    else {
                        rotate = true;
                    }
                }

                draggedTemplate.draw(snapX,snapY, rotate);
                DrawTexture(draggedTemplate.textura,snapX-GRID_SIZE*3,snapY-GRID_SIZE*2,draggedTemplate.color);

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !pause && money >= draggedTemplate.price)
                {// tohle se musi vymyslet nejak jinak ale pro představu je to zatim takhle
                    int HouseGridX;
                    int HouseGridY;

                    if (rotate) {
                        HouseGridX = (snapX - draggedTemplate.gridHeight/2*GRID_SIZE + gridArea) / GRID_SIZE;
                        HouseGridY = (snapY - draggedTemplate.gridWidth/2*GRID_SIZE + gridArea) / GRID_SIZE;
                        rotatedWidth = draggedTemplate.gridHeight;
                        rotatedHeight = draggedTemplate.gridWidth;
                    }
                    else {
                        HouseGridX = (snapX - draggedTemplate.gridWidth/2*GRID_SIZE + gridArea) / GRID_SIZE;
                        HouseGridY = (snapY - draggedTemplate.gridHeight/2*GRID_SIZE + gridArea) / GRID_SIZE;
                        rotatedWidth = draggedTemplate.gridWidth;
                        rotatedHeight = draggedTemplate.gridHeight;
                    }
                    
                    for (int x = 0; x < rotatedWidth; x++)
                    {
                        for (int y = 0; y < rotatedHeight; y++)
                        {
                            int targetX = HouseGridX + x;
                            int targetY = HouseGridY + y;

                            if (targetX >= 0 && targetX < cells && targetY >= 0 && targetY < cells)
                            {
                                grid[targetX][targetY].barv = draggedTemplate.color;
                            }
                        }
                    }
                    money -= draggedTemplate.price;
                    draggedTemplate.SetPosition(snapX-GRID_SIZE*(draggedTemplate.gridWidth/2),snapY-GRID_SIZE*(draggedTemplate.gridHeight/2));
                    ConstructedBuildings.emplace_back(draggedTemplate);

                    if (shops) {
                        activeShops.push_back((Vector2){(float)snapX, (float)snapY});
                    }
                    else if (houses) {
                        houseslocations.push_back((Vector2){(float)snapX, (float)snapY});
                    }
                    isdragg = 0;
                }
                else if (money < draggedTemplate.price) {
                    no_money = true;
                }
            }
            else {
                no_money = false;
            }

            //draw buildings
            for (auto& budova : ConstructedBuildings) {
                budova.drawTexture();
            }

            EndMode2D();

            if (no_money){
                DrawText("Not enough money",  screenWidth/2 - 200, screenHeight/2, 40, WHITE);
            }    

            //in game menu
            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));
            DrawRectangle(0, 0, screenWidth, 50, Color(BROWN));

            float fromtop = 50;


            GuiValueBox((Rectangle){500, 0, 100, 50}, "Money", &money, 0, 100, false);
            GuiValueBox((Rectangle){650, 0, 100, 50}, "Pople", &alive_npc, 0, 10000, false);


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
            if (GuiButton((Rectangle){0, fromtop+200, 100, 50}, "Spawn NPC") && !pause){
                alive_npc++;
                npc1[alive_npc] = NPC();
                npc1[alive_npc].x = 0;
                npc1[alive_npc].y = 0;
                npc1[alive_npc].speedX = 0;
                npc1[alive_npc].speedY = 0;
                npc1[alive_npc].rad = 15;
                npc1[alive_npc].amount = npc1->amount + 1;
                npc1[alive_npc].work = NONE;
                npc1[alive_npc].doing = NPC_IDLE;
                npc1[alive_npc].age = 20;
                npc1[alive_npc].clicked = false;
                npc1[alive_npc].hasahouse = false;
                sprintf(npc1[alive_npc].name, "NPC %d", npc1->amount++);
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
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Food Shop")){
                        draggedTemplate = BuildingTemplate[2];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Goods Shop")){
                        draggedTemplate = BuildingTemplate[3];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "CANCEL")){
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
                    shops = false;
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
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 2*buttonHeight + pausemenuspacing, buttonWidth, buttonHeight}, "Save Game")){
                    ofstream myfile("save.txt");
                    if (myfile.is_open()) {
                        for (int i = 0; i < cells; i++) {
                            for (int j = 0; j < cells; j++) {
                                // Only save if the tile isn't empty
                                if (grid[i][j].barv.a != 0) { 
                                    // Save: GridX GridY R G B A
                                    myfile << i << " " << j << " " 
                                    << (int)grid[i][j].barv.r << " " 
                                    << (int)grid[i][j].barv.g << " " 
                                    << (int)grid[i][j].barv.b << " " 
                                    << (int)grid[i][j].barv.a << "\n";
                                }
                            }
                        }
                        myfile.close();
                    }
                }
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 3*buttonHeight + 2*pausemenuspacing, buttonWidth, buttonHeight }, "Load game")) {
                    DrawRectangle(pausemenuX, pausemenuY, pausemenuW, pausemenuH, Color{BROWN});


                    // ifstream myfile ("save.txt");
                    // if (myfile.is_open()) {
                    //     // 1. Reset grid to blank first
                    //     for (int i = 0; i < cells; i++) {
                    //         for (int j = 0; j < cells; j++) {
                    //             grid[i][j].barv = TerrainColors[TERRAIN_BLANK];
                    //         }
                    //     }

                    //     // 2. Read only the objects that were saved
                    //     int x, y, r, g, b, a;
                    //     while (myfile >> x >> y >> r >> g >> b >> a) {
                    //         if (x >= 0 && x < cells && y >= 0 && y < cells) {
                    //             grid[x][y].barv = {(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
                    //         }
                    //     }
                    //     myfile.close();
                    //     run = true;
                    // }
                }
                if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 4*buttonHeight + 3*pausemenuspacing, buttonWidth, buttonHeight }, "To Main Menu")) {
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