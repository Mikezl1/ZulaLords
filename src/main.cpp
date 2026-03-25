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
#include "materials.h"
using namespace std;


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

void SmartBlock(std::vector<std::vector<Object>>& grid, int x, int y) {
    auto isSame = [&](int cx, int cy) {
        if (cx < 0 || cx >= grid.size() || cy < 0 || cy >= grid[0].size()) return true; 
        return grid[cx][cy].barv == grid[x][y].barv;
    };

    bool U = isSame(x, y - 1);
    bool D = isSame(x, y + 1);
    bool L = isSame(x - 1, y);
    bool R = isSame(x + 1, y);

    bool UL = U && L && isSame(x - 1, y - 1);
    bool UR = U && R && isSame(x + 1, y - 1);
    bool DL = D && L && isSame(x - 1, y + 1);
    bool DR = D && R && isSame(x + 1, y + 1);

    auto& tex = grid[x][y].textura;
    int sum = U + D + L + R;

    if (sum == 4) {
        int diagSum = UL + UR + DL + DR;
        
        if (diagSum == 4) grid[x][y].drawTextures(tex.full);
        else if (diagSum == 3) {
            if (!UL) grid[x][y].drawTextures(tex.full_notleftup);
            else if (!UR) grid[x][y].drawTextures(tex.full_notrightup);
            else if (!DL) grid[x][y].drawTextures(tex.full_notleftdown);
            else if (!DR) grid[x][y].drawTextures(tex.full_notrightdown);
        }
        else if (diagSum == 2) {
            if (!UL && !UR) grid[x][y].drawTextures(tex.full_not_top_corners);
            else if (!DL && !DR) grid[x][y].drawTextures(tex.full_not_bottom_corners);
            else if (!UL && !DL) grid[x][y].drawTextures(tex.full_not_left_corners);
            else if (!UR && !DR) grid[x][y].drawTextures(tex.full_not_right_corners);
            else if (!UL && !DR) grid[x][y].drawTextures(tex.full_not_diag_1);
            else if (!UR && !DL) grid[x][y].drawTextures(tex.full_not_diag_2);
        }
        else if (diagSum == 1) {
            if (UL) grid[x][y].drawTextures(tex.full_only_leftup);
            else if (UR) grid[x][y].drawTextures(tex.full_only_rightup);
            else if (DL) grid[x][y].drawTextures(tex.full_only_leftdown);
            else if (DR) grid[x][y].drawTextures(tex.full_only_rightdown);
        }
        else { 
            grid[x][y].drawTextures(tex.full_not_all_corners);
        }
    }
    else if (sum == 3) {
        if (!U) { 
            if (DL && DR) grid[x][y].drawTextures(tex.notup);
            else if (!DL && DR) grid[x][y].drawTextures(tex.notup_notleftdown);
            else if (DL && !DR) grid[x][y].drawTextures(tex.notup_notrightdown);
            else grid[x][y].drawTextures(tex.notup_notboth);
        }
        else if (!D) { 
            if (UL && UR) grid[x][y].drawTextures(tex.notdown);
            else if (!UL && UR) grid[x][y].drawTextures(tex.notdown_notleftup);
            else if (UL && !UR) grid[x][y].drawTextures(tex.notdown_notrightup);
            else grid[x][y].drawTextures(tex.notdown_notboth);
        }
        else if (!L) { 
            if (UR && DR) grid[x][y].drawTextures(tex.notleft);
            else if (!UR && DR) grid[x][y].drawTextures(tex.notleft_notrightup);
            else if (UR && !DR) grid[x][y].drawTextures(tex.notleft_notrightdown);
            else grid[x][y].drawTextures(tex.notleft_notboth);
        }
        else if (!R) { 
            if (UL && DL) grid[x][y].drawTextures(tex.notright);
            else if (!UL && DL) grid[x][y].drawTextures(tex.notright_notleftup);
            else if (UL && !DL) grid[x][y].drawTextures(tex.notright_notleftdown);
            else grid[x][y].drawTextures(tex.notright_notboth);
        }
    }
    else if (sum == 2) {
        if (U && D) grid[x][y].drawTextures(tex.vertically);
        else if (L && R) grid[x][y].drawTextures(tex.horizontal);
        else if (D && R) grid[x][y].drawTextures(DR ? tex.corner_leftup_F : tex.corner_leftup_E);
        else if (D && L) grid[x][y].drawTextures(DL ? tex.corner_rightup_F : tex.corner_rightup_E);
        else if (U && R) grid[x][y].drawTextures(UR ? tex.corner_leftdown_F : tex.corner_leftdown_E);
        else if (U && L) grid[x][y].drawTextures(UL ? tex.corner_rightdown_F : tex.corner_rightdown_E);
    }
    else if (sum == 1) {
        if (R) grid[x][y].drawTextures(tex.left);     
        else if (L) grid[x][y].drawTextures(tex.right);
        else if (D) grid[x][y].drawTextures(tex.up);   
        else if (U) grid[x][y].drawTextures(tex.down);  
    }
    else { 
        grid[x][y].drawTextures(tex.center);
    }
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
            if (grid[i][j].haveTexture)
            {
                SmartBlock(grid, i, j);
            }
            else
            {
                grid[i][j].draw();
            }
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

    float timer = 0.0f;

    bool run = false;
    bool pause = false;
    bool settings = false;
    bool build = false;
    bool paths = false;
    bool zones = false;
    bool destroy = false;
    bool shops = false;
    bool walls = false;
    bool no_money = false;
    bool showZones = false;
    

    int mousehold = 0;

    int money = 1000;




    //building stuff
    std::vector<ZoneTemplate> LiveZone;
    LiveZone.reserve(100);
    bool isdragg = 0;
    int draggedZone;


    TexPack StoneWall;
    StoneWall = LoadTexPack("BasicWallmodel22.png",BLUE);
    TexPack WoodenWall;
    WoodenWall = LoadTexPack("better wooden wall.png",BLUE);
    std::vector<NPC> npc1(10000);
    int alive_npc = 0;

    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    std::vector<Materials> materials;
    materials.reserve(100);

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
                timer += GetFrameTime();
                for(int i = 0; i < alive_npc+1; i++) {
                    npc1[i].NPC_movement(LiveZone, grid);
                    if (npc1[i].doing == NPC_WORKING) {
                        if (timer >= 5.0f) {
                            bool foundStack = false;

                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (materials[ii].x == npc1[i].workplaceX && materials[ii].y == npc1[i].workplaceY && materials[ii].type == WOOD) {
                                    materials[ii].amount++;
                                    foundStack = true;
                                    TraceLog(LOG_INFO, "MATERIAL ADDED at Grid: %d, %d. Total: %d", materials[ii].x, materials[ii].y, materials[ii].amount);
                                    timer = 0.0f;
                                    break;
                                }
                            }                          
                            
                            if (!foundStack || materials.empty()) {
                                Materials newMat;
                                newMat.type = WOOD;
                                newMat.amount = 1;
                                newMat.x = npc1[i].workplaceX;
                                newMat.y = npc1[i].workplaceY;
                                newMat.Barva = BLUE;
                                newMat.picked_up = false;
                                
                                materials.push_back(newMat);
                                timer = 0.0f;
                                TraceLog(LOG_INFO, "MATERIAL SPAWNED at Grid: %d, %d. Total: %d", newMat.x, newMat.y, (int)materials.size());
                            }
                        }
                    }
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

            for (int i = 0; i < (int)materials.size(); i++) {
                materials[i].Draw();
            }
            
            for(int i = 0; i < alive_npc+1; i++) {
                npc1[i].draw();
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

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !pause && (paths | destroy | walls | zones) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, (float)screenWidth, 50}))
            {
                int StartX = (gridX < StartGridX) ? gridX : StartGridX;// aby to šlo do minusu
                int EndX = (gridX > StartGridX) ? gridX : StartGridX;
                int StartY = (gridY < StartGridY) ? gridY : StartGridY;// aby to šlo do minusu
                int EndY = (gridY > StartGridY) ? gridY : StartGridY;

                ZoneTemplate newZone;
                int currentZoneIndex = 0;
                if (zones && draggedZone != CLEAR)
                {
                    newZone.who_am_I = draggedZone;
                    LiveZone.emplace_back(newZone);
                    currentZoneIndex = LiveZone.size() - 1;
                    LiveZone[currentZoneIndex].startX = grid[StartX][StartY].drawX;
                    LiveZone[currentZoneIndex].startY = grid[StartX][StartY].drawY;
                    LiveZone[currentZoneIndex].endX = grid[EndX][EndY].drawX;
                    LiveZone[currentZoneIndex].endY = grid[EndX][EndY].drawY;

                    LiveZone[currentZoneIndex].zoneIndex = currentZoneIndex;
                }

                for (int i = StartX; i <= EndX; i++)
                {
                    for (int ii = StartY; ii <= EndY; ii++)
                    {
                        if (i >= 0 && i < cells && ii >= 0 && ii < cells ) 
                        {
                            if (destroy) 
                            {
                                grid[i][ii].barv = TerrainColors[TERRAIN_BLANK];
                                grid[i][ii].haveTexture = false;
                            }
                            
                            if (paths)
                            {
                                grid[i][ii].barv = TerrainColors[mousehold];
                                grid[i][ii].haveTexture = false;                      
                            }
                            
                            if (zones)
                            {
                                if (draggedZone == CLEAR)
                                {
                                    if (grid[i][ii].am_I_zone == true) 
                                    {
                                        int oldZoneID = grid[i][ii].myzone; 
                                        
                                        grid[i][ii].am_I_zone = false;      

                                        for (auto it = LiveZone[oldZoneID].ownedCells.begin(); it != LiveZone[oldZoneID].ownedCells.end(); ) {
                                            if (it->x == i && it->y == ii) {
                                                it = LiveZone[oldZoneID].ownedCells.erase(it);
                                            } else {
                                                ++it;
                                            }
                                        }
                                    }
                                }
                                else if (grid[i][ii].am_I_zone == false && grid[i][ii].haveTexture == false) {
                                    grid[i][ii].am_I_zone = true;
                                    grid[i][ii].myzone = currentZoneIndex;
                                    
                                    grid[i][ii].what_am_I = draggedZone;
                                    
                                    LiveZone[currentZoneIndex].ownedCells.push_back({i, ii});
                                    LiveZone[currentZoneIndex].who_am_I = draggedZone;
                                    LiveZone[currentZoneIndex].type = (ZoneType)draggedZone;
                                }
                            
                            }
                            if (walls) 
                            {
                                if (mousehold == 0) // DESTROYING WALLS
                                {
                                    for (int i = StartX; i <= EndX; i++) {
                                        for (int ii = StartY; ii <= EndY; ii++) {
                                            if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                                                grid[i][ii].textura = TexPack(); 
                                                grid[i][ii].haveTexture = false;
                                                grid[i][ii].barv = TerrainColors[TERRAIN_BLANK];
                                            }
                                        }
                                    }
                                }
                                else // BUILDING WALLS
                                {
                                    int validEmptyTiles = 0;
                                    int costPerWall = 2;

                                    // Step A: Dry Run (Count the empty tiles)
                                    for (int i = StartX; i <= EndX; i++) {
                                        for (int ii = StartY; ii <= EndY; ii++) {
                                            if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                                                if (grid[i][ii].haveTexture == false) {
                                                    validEmptyTiles++;
                                                }
                                            }
                                        }
                                    }

                                    int totalCost = validEmptyTiles * costPerWall;

                                    if (money >= totalCost && validEmptyTiles > 0)
                                    {
                                        no_money = false;
                                        money -= totalCost;

                                        for (int i = StartX; i <= EndX; i++) {
                                            for (int ii = StartY; ii <= EndY; ii++) {
                                                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                                                    if (grid[i][ii].haveTexture == false) 
                                                    {
                                                        grid[i][ii].haveTexture = true;
                                                        grid[i][ii].barv = TerrainColors[mousehold];
                                                        
                                                        if (mousehold == Wall_Stone) {
                                                            grid[i][ii].textura = StoneWall;
                                                        } 
                                                        else if (mousehold == Wall_Wooden) {
                                                            grid[i][ii].textura = WoodenWall; 
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (validEmptyTiles > 0)
                                    {
                                        no_money = true; 
                                    }
                                }
                            }
                        }    
                    }
                }
                if (zones && draggedZone != CLEAR)
                {
                    std::vector<int> zonesToMergeWith;

                    for (auto& p : LiveZone[currentZoneIndex].ownedCells) 
                    {
                        auto checkNeighbor = [&](int nx, int ny) {
                            if (nx >= 0 && nx < cells && ny >= 0 && ny < cells) {
                                if (grid[nx][ny].am_I_zone == true && grid[nx][ny].myzone != currentZoneIndex)
                                {
                                    int foundId = grid[nx][ny].myzone;
                                    
                                    if (LiveZone[foundId].who_am_I == draggedZone) 
                                    {
                                        bool alreadyFound = false;
                                        for (int id : zonesToMergeWith) {
                                            if (id == foundId) alreadyFound = true;
                                        }
                                        if (!alreadyFound) zonesToMergeWith.push_back(foundId);
                                    }
                                }
                            }
                        };

                        // Check Up, Down, Left, Right
                        checkNeighbor(p.x, p.y - 1);
                        checkNeighbor(p.x, p.y + 1);
                        checkNeighbor(p.x - 1, p.y);
                        checkNeighbor(p.x + 1, p.y);
                    }

                    if (!zonesToMergeWith.empty()) 
                    {
                        // We will dump everything into the FIRST older zone we touched
                        int masterZoneId = zonesToMergeWith[0];

                        // Dump the NEW zone we just drew into the master zone
                        for (auto& p : LiveZone[currentZoneIndex].ownedCells) {
                            LiveZone[masterZoneId].ownedCells.push_back(p);
                            grid[p.x][p.y].myzone = masterZoneId; // Update the grid to point to the master!
                        }
                        // Empty the new zone so it becomes a ghost
                        LiveZone[currentZoneIndex].ownedCells.clear(); 

                        // What if our new drag bridged TWO OR MORE existing zones together?
                        // We need to dump those other older zones into the master zone too!
                        for (size_t k = 1; k < zonesToMergeWith.size(); k++) 
                        {
                            int otherZoneId = zonesToMergeWith[k];
                            for (auto& p : LiveZone[otherZoneId].ownedCells) {
                                LiveZone[masterZoneId].ownedCells.push_back(p);
                                grid[p.x][p.y].myzone = masterZoneId; // Update grid
                            }
                            // Empty the bridged zone
                            LiveZone[otherZoneId].ownedCells.clear(); 
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
            
            for (auto& zone : LiveZone) {/// vykresluje svetle modou na zony
                zone.CheckValidity(grid);
                if (showZones || !zone.valid)
                {
                    zone.draw(camera, grid);
                }
            }                

            EndMode2D();

            if (no_money){
                DrawText("Not enough money",  screenWidth/2 - 200, screenHeight/2, 40, WHITE);
            }    

            //in game menu
            DrawRectangle(0, 0, screenWidth, 50, Color(BROWN));
            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));

            float fromtop = 50;


            GuiValueBox((Rectangle){500, 0, 100, 50}, "Money", &money, 0, 100, false);
            GuiValueBox((Rectangle){650, 0, 100, 50}, "Pople", &alive_npc, 0, 10000, false);


            if (GuiButton((Rectangle){0, fromtop, 100, 50}, "PATHS") && !build){
                paths = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, "ZONES") && !build){
                zones = true;
                build = true;
                showZones = true;
            }
            if (GuiButton((Rectangle){0, fromtop+100, 100, 50}, "SHOPS") && !build){
                build = true;
                shops = true;
            }
            if (GuiButton((Rectangle){0, fromtop+150, 100, 50}, "WALLS  ") && !build){
                build = true;
                walls = true;
            }

            if (GuiButton((Rectangle){0, fromtop+200, 100, 50}, "DESTROY") && !build){
                destroy = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+250, 100, 50}, "Spawn NPC") && !pause){
                alive_npc++;
                npc1[alive_npc] = NPC();
                npc1[alive_npc].x = 0;
                npc1[alive_npc].y = 0;
                npc1[alive_npc].speedX = 0;
                npc1[alive_npc].speedY = 0;
                npc1[alive_npc].rad = 15;
                npc1[alive_npc].amount = npc1[alive_npc].amount + 1;
                npc1[alive_npc].work = NONE;
                npc1[alive_npc].doing = NPC_IDLE;
                npc1[alive_npc].age = 20;
                npc1[alive_npc].clicked = false;
                npc1[alive_npc].hasahouse = false;
                sprintf(npc1[alive_npc].name, "NPC %d", npc1[alive_npc].amount++);
            }

            if (GuiButton((Rectangle){(float)(screenWidth - 150), 0, 100.0f, 50.0f}, "Show zones") && !pause){
                showZones = !showZones;
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

                if(zones) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "HOUSE ZONE")){
                        draggedZone = HOUSE_ZONE;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "SHOP ZONE")){
                        draggedZone = SHOP_ZONE;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "WORK ZONE")){
                        draggedZone = WORK_ZONE;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "MERCHANT ZONE")){
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 5*fromtop, 100, 50}, "CLEAR ZONES")){
                        draggedZone = CLEAR;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 8*fromtop, 100, 50}, "CANCEL")){
                        zones = false;
                        build = false;
                        showZones = false;
                    }
                }

                if (shops) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Food Shop")){
                        //draggedZone = BuildingTemplate[2];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Goods Shop")){
                        //draggedZone = BuildingTemplate[3];
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "CANCEL")){
                        shops = false;
                        build = false;
                    }
                }

                if (walls) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Stone wall")){
                        mousehold = Wall_Stone;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Wooden wall")){
                        mousehold = Wall_Wooden;
                        isdragg = 1;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "CANCEL")){
                        walls = false;
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
                    zones = false;
                    shops = false;
                    destroy = false;
                    build = false;
                    walls = false;
                    showZones = false;
                    no_money = false;
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