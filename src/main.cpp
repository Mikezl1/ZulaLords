#include <raylib.h>
#include "raymath.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION

#include "raygui.h"

#include "constants.h"
#include "npc.h"
#include "buildings.h"
#include "materials.h"
using namespace std;


void gridSetup (std::vector<std::vector<Object>>& grid, std::vector<Object*>& treesRegistry, std::vector<Object*>& stoneRegistry) {
    for (int x = 0; x < cells; x++) {
        for (int y = 0; y < cells; y++) {
            grid[x][y].barv = TerrainColors[TERRAIN_BLANK];
            grid[x][y].materialsource = NO_SOURCE;
            grid[x][y].am_I_zone = false;
            grid[x][y].built = false;
            grid[x][y].x = x;
            grid[x][y].y = y;
            grid[x][y].drawX = (x * GRID_SIZE) - gridArea; 
            grid[x][y].drawY = (y * GRID_SIZE) - gridArea;

        }
    }

    int a = GetRandomValue(70, 300);
    for (int t = 0; t < a; t++) {
        int r = GetRandomValue(20, 100);
        int x = GetRandomValue(-cells, cells);
        int y = GetRandomValue(-cells, cells);

        TraceLog(LOG_INFO, "Spawned forest at X:%d Y:%d", x, y);


        for (int i = x; i < x + r; i++) {
            for (int ii = y; ii < y + r; ii++) {
                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                    if(i % 2 && ii % 2 && grid[i][ii].materialsource == NO_SOURCE) {
                        grid[i][ii].barv = DARKGREEN;
                        grid[i][ii].materialsource = TREE;
                        grid[i][ii].haveTexture = false; // Prozatím nemá texturu, až nějakou udělám tak ji bude mít

                        treesRegistry.push_back(&grid[i][ii]);
                    }
                }
            }   
        }
    } 
    
    a = GetRandomValue(50, 250);
    for (int t = 0; t < a; t++) {
        int r = GetRandomValue(5, 40);
        int x = GetRandomValue(-cells, cells);
        int y = GetRandomValue(-cells, cells);

        TraceLog(LOG_INFO, "Spawned rocks at X:%d Y:%d", x, y);


        for (int i = x; i < x + r; i++) {
            for (int ii = y; ii < y + r; ii++) {
                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                    if(grid[i][ii].materialsource == NO_SOURCE) {
                        grid[i][ii].barv = DARKGRAY;
                        grid[i][ii].materialsource = ROCK;
                        grid[i][ii].haveTexture = false; // Prozatím nemá texturu, až nějakou udělám tak ji bude mít

                        stoneRegistry.push_back(&grid[i][ii]);
                    }
                }
            }   
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
        if (cx < 0 || cx >= (int)grid.size() || cy < 0 || cy >= (int)grid[0].size()) return true; 
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

    int draggedZone;
    int itemIDgen = 0;

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
    bool work_zones = false;
    bool no_money = false;
    bool showZones = false;
    bool notplacable = false;
    // 5bool isdragg = false;
    

    int mousehold = 0;

    int money = 10000;




    //building stuff
    std::vector<ZoneTemplate> LiveZone;
    LiveZone.reserve(100);

    TexPack StoneWall;
    StoneWall = LoadTexPack("BasicWallmodel22.png",BLUE);
    TexPack WoodenWall;
    WoodenWall = LoadTexPack("better wooden wall.png",BLUE);
    std::vector<NPC> npc1(10000);
    int alive_npc = 0;

    std::vector<std::vector<Object>> grid(cells, std::vector<Object>(cells));

    std::vector<Object*> wallRegistry;
    std::vector<Object*> treesRegistry;
    std::vector<Object*> stoneRegistry;

    std::vector<Materials> materials;

    materials.reserve(1000);
    //grid setup
    gridSetup(grid, treesRegistry, stoneRegistry);

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
                for (int i = 0; i < 50; i++) {
                    Materials newMat;
                    newMat.type = WOOD;
                    newMat.x = GetRandomValue(-200, 200);
                    newMat.y = GetRandomValue(-200, 200);
                    newMat.Barva = BLUE;
                    newMat.walking_towards = false;
                    newMat.id = itemIDgen++;
                        
                    materials.push_back(newMat);
                }
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
            

            if (!pause && !npc1.empty()) {
                timer += GetFrameTime();
                for(int i = 0; i < alive_npc; i++) {
                    if (!npc1[i].registeredhouse && npc1[i].hasahouse) {
                        for (auto& zone : LiveZone) {
                            if (zone.capacity > 0 && npc1[i].homeX >= zone.startX && npc1[i].homeX <= zone.endX && npc1[i].homeY >= zone.startY && npc1[i].homeY <= zone.endY && zone.type == HOUSE_ZONE) {

                                zone.capacity--;
                                npc1[i].registeredhouse = true;
                                
                                TraceLog(LOG_INFO, "NPC %d moved into house %d! Remaining capacity: %d", i, zone.zoneIndex, zone.capacity);
                            }
                        }
                    }

                    npc1[i].NPC_movement(LiveZone, grid);
                    bool somethingtobuild = false;

                    for (auto* wall : wallRegistry) { 
                        if (wall->built == false && wall->haveTexture && !wallRegistry.empty() && !wall->walking_towards) {
                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (wall->madeoutof == WOOD && materials[ii].type == WOOD) {
                                    npc1[i].itemType = WOOD;
                                }
                                else if (wall->madeoutof == STONE && materials[ii].type == STONE) {
                                    npc1[i].itemType = STONE;
                                }
                            }
                            
                            somethingtobuild = true;
                        }
                    }

                    if (npc1[i].doing == NPC_IDLE && !npc1[i].holdingitem && somethingtobuild) {
                        int bestIdx = -1;
                        float closestDist = -1.0f;
                        Vector2 currentPos = {(float)npc1[i].x, (float)npc1[i].y};
                        

                        for (int ii = 0; ii < (int)materials.size(); ii++) {
                            if (!materials[ii].walking_towards && !materials[ii].picked_up && materials[ii].type == npc1[i].itemType) {
                                float d = Vector2Distance(currentPos, {(float)materials[ii].x, (float)materials[ii].y});
                                
                                if (closestDist < 0 || d < closestDist) {
                                    closestDist = d;
                                    bestIdx = ii;
                                }
                            }
                        }

                        if (bestIdx != -1) {
                            npc1[i].destinationX = materials[bestIdx].x;
                            npc1[i].destinationY = materials[bestIdx].y;
                            npc1[i].itemID = materials[bestIdx].id;
                            npc1[i].doing = NPC_GATHERING;
                            materials[bestIdx].walking_towards = true; // Reserve it
                            TraceLog(LOG_INFO, "NPC %d targeting closest material (ID: %d)", i, materials[bestIdx].id);
                        }
                    }
                

                    if (npc1[i].doing == NPC_GATHERING) {
                        if (Vector2Distance({(float)npc1[i].x, (float)npc1[i].y}, {(float)npc1[i].destinationX, (float)npc1[i].destinationY}) < 5.0f) {
                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (materials[ii].id == npc1[i].itemID) {
                                    materials[ii].picked_up = true;
                                    npc1[i].holdingitem = true;
                                    npc1[i].doing = NPC_IDLE;
                                    npc1[i].itemType = materials[ii].type;
                                    break;
                                }
                            }
                        }
                    }

                    if (npc1[i].doing == NPC_IDLE && npc1[i].holdingitem) {
                        for (auto* wall : wallRegistry) {
                            if (wall->haveTexture && !wall->built && wall->madeoutof == npc1[i].itemType && !wall->walking_towards) {
                                npc1[i].destinationX = wall->drawX + GRID_SIZE / 2;
                                npc1[i].destinationY = wall->drawY + GRID_SIZE / 2;
                                npc1[i].doing = NPC_BUILDING;
                                wall->walking_towards = true;
                                break; 
                            }
                        }
                    }

                    if (npc1[i].doing == NPC_BUILDING) {
                        if (Vector2Distance({(float)npc1[i].x, (float)npc1[i].y}, {(float)npc1[i].destinationX, (float)npc1[i].destinationY}) < 10.0f) {
                            for (auto* wall : wallRegistry) {
                                if (!wall->built && (wall->drawX + GRID_SIZE/2 == npc1[i].destinationX)) {
                                    for (int m = 0; m < (int)materials.size(); m++) {
                                        if (materials[m].id == npc1[i].itemID) {
                                            materials.erase(materials.begin() + m);
                                            break;
                                        }
                                    }

                                    wall->built = true;
                                    npc1[i].holdingitem = false;
                                    npc1[i].doing = NPC_IDLE;
                                    break;
                                }
                            }
                        }
                    }


                    if (npc1[i].doing == NPC_WORKING || npc1[i].doing == NPC_LOGGING || npc1[i].doing == NPC_FARMING || npc1[i].doing == NPC_MINING) {
                        if (npc1[i].work == SAWMILL && !npc1[i].found_source) {
                            npc1[i].doing = NPC_LOGGING;
                            bool found = false;
                            int treeX;
                            int treeY;
                            float closestDist = -1.0f;
                            Vector2 currentPos = {(float)npc1[i].x, (float)npc1[i].y};

                            for (auto& trees : treesRegistry) {
                                if (!trees->walking_towards) {
                                    float d = Vector2Distance(currentPos, {(float)trees->drawX, (float)trees->drawY});

                                    if (closestDist < 0 || d < closestDist) {
                                        closestDist = d;
                                        found = true;
                                        treeX = trees->drawX;
                                        treeY = trees->drawY;
                                        trees->walking_towards = true;
                                    }
                                    else {
                                        trees->walking_towards = false;
                                    }
                                }
                            }


                            
                            if (found) {
                                npc1[i].startX = npc1[i].x;
                                npc1[i].startY = npc1[i].y;
                                npc1[i].destinationX = treeX + GRID_SIZE/2;
                                npc1[i].destinationY = treeY + GRID_SIZE/2;
                                npc1[i].found_source = true;
                                TraceLog(LOG_INFO, "NPC %d targeting closest tree at X: %d Y: %d", i, treeX, treeY);
                            }

                            if (npc1[i].found_source) {
                                for (auto& trees : treesRegistry) {
                                    if (npc1[i].x >= trees->drawX && npc1[i].y >= trees->drawY && npc1[i].x <= trees->drawX + GRID_SIZE && npc1[i].y <= trees->drawY + GRID_SIZE) {
                                        //grid[trees->drawX][trees->drawY].barv = BLANK;
                                        trees->barv = BLANK;
                                        trees->materialsource = NO_SOURCE;
                                        treesRegistry.back();
                                        treesRegistry.pop_back();

                                        Materials newMat;
                                        newMat.type = WOOD;
                                        newMat.x = npc1[i].x;
                                        newMat.y = npc1[i].y;
                                        newMat.Barva = BLUE;
                                        newMat.walking_towards = false;
                                        newMat.id = itemIDgen++;
                                            
                                        materials.push_back(newMat);

                                        TraceLog(LOG_INFO, "NPC %d got to a tree at X: %d Y: %d and chopped it down", i, treeX, treeY);
                                    }
                                }
                            }
                        }

                        if (npc1[i].doing == NPC_LOGGING || npc1[i].doing == NPC_MINING || npc1[i].doing == NPC_FARMING) {
                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (npc1[i].x == materials[ii].x && npc1[i].y == materials[ii].y && materials[ii].walking_towards && !materials[ii].picked_up) {
                                    materials[ii].picked_up = true;
                                    materials[ii].walking_towards = true;
                                    npc1[i].holdingitem = true;
                                    npc1[i].itemID = materials[ii].id;
                                }
                            }

                            if (npc1[i].holdingitem) {
                                npc1[i].startX = npc1[i].x;
                                npc1[i].startY = npc1[i].y;
                                npc1[i].destinationX = npc1[i].workplaceX;
                                npc1[i].destinationY = npc1[i].workplaceY;

                                if (npc1[i].x == npc1[i].workplaceX && npc1[i].y == npc1[i].workplaceY) {
                                    for(int ii = 0; ii < (int)materials.size(); ii++) {
                                        if (npc1[i].itemID == materials[ii].id) {
                                            materials[ii].picked_up = false;
                                            materials[ii].walking_towards = false;
                                            npc1[i].itemID = 0;
                                            break;
                                        }
                                    }
                                    npc1[i].holdingitem = false;
                                    npc1[i].finished_work = true;
                                }
                                else {
                                    npc1[i].finished_work = false;
                                }
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
            
            for(int i = 0; i < alive_npc; i++) {
                npc1[i].draw();
            }

            for (int i = 0; i < (int)materials.size(); i++) {
                if (!npc1.empty() && materials[i].picked_up) {
                    for (int ii = 0; ii < (int)npc1.size() + 1; ii++) {
                        if (npc1[ii].holdingitem && npc1[ii].itemID == materials[i].id) {
                            materials[i].x = npc1[ii].x;
                            materials[i].y = npc1[ii].y;
                        }
                    }
                }
                materials[i].Draw();
            }
// 
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

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !pause && (paths | destroy | walls | zones) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, (float)screenWidth, 50}) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){100, 130, 100, 140}))
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
                                if (grid[i][ii].haveTexture) { 
                                    if (grid[i][ii].built) {
                                        Materials droppedMat;
                                        droppedMat.type = WOOD;
                                        droppedMat.x = (float)grid[i][ii].drawX + (GRID_SIZE / 2);
                                        droppedMat.y = (float)grid[i][ii].drawY + (GRID_SIZE / 2);
                                        droppedMat.Barva = BLUE;
                                        droppedMat.walking_towards = false;
                                        droppedMat.picked_up = false;
                                        droppedMat.id = itemIDgen++;
                                        
                                        materials.push_back(droppedMat);
                                        TraceLog(LOG_INFO, "Wall destroyed at %d, %d. Material dropped.", i, ii);
                                    }

                                    Object* target = &grid[i][ii];
                                    grid[i][ii].haveTexture = false;
                                    grid[i][ii].barv = TerrainColors[TERRAIN_BLANK];
                                    grid[i][ii].built = false;
                                    for (int n = 0; n < (int)wallRegistry.size(); n++) {
                                        if (wallRegistry[n] == target) {
                                            wallRegistry[n] = wallRegistry.back();
                                            wallRegistry.pop_back();

                                            break;
                                        }
                                    }
                                }
                                else {
                                    if (grid[i][ii].materialsource == NO_SOURCE) {
                                        grid[i][ii].barv = TerrainColors[TERRAIN_BLANK];
                                    }
                                }
                            }
                            
                            if (paths)
                            {   
                                if (grid[i][ii].materialsource == NO_SOURCE) {
                                    grid[i][ii].barv = TerrainColors[mousehold];
                                    grid[i][ii].haveTexture = false;
                                }
                                else {
                                    notplacable = true;
                                }
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
                                    if (draggedZone == HOUSE_ZONE) {
                                        LiveZone[currentZoneIndex].capacity = 4;
                                    }
                                }
                            
                            }
                            if (walls) 
                            {
                                if (mousehold != 0)  // BUILDING WALLS
                                {
                                    int validEmptyTiles = 0;

                                    for (int i = StartX; i <= EndX; i++) {
                                        for (int ii = StartY; ii <= EndY; ii++) {
                                            if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                                                if (grid[i][ii].haveTexture == false && grid[i][ii].materialsource == NO_SOURCE) {
                                                    validEmptyTiles++;
                                                }
                                            }
                                        }
                                    }

                                    if (validEmptyTiles > 0)
                                    {
                                        notplacable = false;
                                        for (int i = StartX; i <= EndX; i++) {
                                            for (int ii = StartY; ii <= EndY; ii++) {
                                                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                                                    if (grid[i][ii].haveTexture == false && grid[i][ii].materialsource == NO_SOURCE) 
                                                    {
                                                        grid[i][ii].built = false;
                                                        grid[i][ii].haveTexture = true;
                                                        grid[i][ii].barv = TerrainColors[mousehold];
                                                        wallRegistry.push_back(&grid[i][ii]);

                                                        TraceLog(LOG_INFO, "Wall spawned at X: %d, Y: %d", i, ii);
                                                        
                                                        if (mousehold == Wall_Stone) {
                                                            grid[i][ii].textura = StoneWall;
                                                            grid[i][ii].madeoutof = STONE;
                                                        } 
                                                        else if (mousehold == Wall_Wooden) {
                                                            grid[i][ii].textura = WoodenWall;
                                                            grid[i][ii].madeoutof = WOOD;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        //notplacable = true;
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

            if (notplacable) {
                DrawText("Something is in the way",  screenWidth/2 - 200, screenHeight/2, 40, WHITE);
            }

            //in game menu
            DrawRectangle(0, 0, screenWidth, 50, Color(BROWN));
            DrawRectangle(0, 0, 100, screenHeight, Color(BROWN));

            float fromtop = 50;


            GuiValueBox((Rectangle){500, 0, 100, 50}, "Money", &money, 0, 10000000, false);
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
                npc1[alive_npc] = NPC();
                npc1[alive_npc].x = 0;
                npc1[alive_npc].y = 0;
                npc1[alive_npc].speedX = 0;
                npc1[alive_npc].speedY = 0;
                npc1[alive_npc].rad = 15;
                npc1[alive_npc].work = NO_WORK;
                npc1[alive_npc].doing = NPC_IDLE;
                npc1[alive_npc].age = 20;
                npc1[alive_npc].clicked = false;
                npc1[alive_npc].hasahouse = false;
                npc1[alive_npc].registeredhouse = false;
                sprintf(npc1[alive_npc].name, "NPC %d", alive_npc);
                alive_npc++;
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
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "SHOP ZONE")){
                        draggedZone = SHOP_ZONE;
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "WORK ZONES") || work_zones){
                        work_zones = true;
                        DrawRectangle(100, 3*fromtop - 20, 100, 140, Color(BROWN));
                        if (GuiButton((Rectangle){100, 3*fromtop, 100, 50}, "SAWMILL ZONE")){
                            draggedZone = WORK_SAWMILL_ZONE;
                        }
                        if (GuiButton((Rectangle){100, 4*fromtop, 100, 50}, "QUARRY ZONE")){
                            draggedZone = WORK_QUARRY_ZONE;
                        }
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "MERCHANT ZONE")){
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 5*fromtop, 100, 50}, "CLEAR ZONES")){
                        draggedZone = CLEAR;
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 8*fromtop, 100, 50}, "CANCEL")){
                        zones = false;
                        build = false;
                        showZones = false;
                        //isdragg = false;
                    }
                }

                if (shops) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Food Shop")){
                        //draggedZone = BuildingTemplate[2];
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Goods Shop")){
                        //draggedZone = BuildingTemplate[3];
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "CANCEL")){
                        shops = false;
                        build = false;
                        //isdragg = false;
                    }
                }

                if (walls) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Stone wall")){
                        mousehold = Wall_Stone;
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Wooden wall")){
                        mousehold = Wall_Wooden;
                        //isdragg = true;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "CANCEL")){
                        walls = false;
                        build = false;
                        //isdragg = false;
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
                    notplacable = false;
                    work_zones = false;
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