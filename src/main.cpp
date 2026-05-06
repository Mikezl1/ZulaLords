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

        for (int i = x; i < x + r; i++) {
            for (int ii = y; ii < y + r; ii++) {
                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                    if(i % 2 && ii % 2 && grid[i][ii].materialsource == NO_SOURCE) {
                        grid[i][ii].barv = DARKGREEN;
                        grid[i][ii].materialsource = TREE;
                        grid[i][ii].haveTexture = false; 

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

        for (int i = x; i < x + r; i++) {
            for (int ii = y; ii < y + r; ii++) {
                if (i >= 0 && i < cells && ii >= 0 && ii < cells) {
                    if(grid[i][ii].materialsource == NO_SOURCE) {
                        grid[i][ii].barv = DARKGRAY;
                        grid[i][ii].materialsource = ROCK;
                        grid[i][ii].haveTexture = false; 

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


Camera2D cameraUpdate(Camera2D camera) {             
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

    Vector2 TopLeft = GetScreenToWorld2D(Vector2{ 0.0f, 0.0f }, camera);
    Vector2 BotRight  = GetScreenToWorld2D(BotRig, camera);

    Vector2 TopLVec = (TopLeft / GRID_SIZE) * GRID_SIZE;
    Vector2 BotRVec = (BotRight / GRID_SIZE) * GRID_SIZE;

    Vector2 TL_drw = Vector2{(TopLVec.x + gridArea) / GRID_SIZE,(TopLVec.y + gridArea) / GRID_SIZE};
    Vector2 BR_drw = Vector2{(BotRVec.x + gridArea) / GRID_SIZE,(BotRVec.y + gridArea) / GRID_SIZE};

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
    Vector2 center = Vector2{ (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };
    return GetScreenToWorld2D(center, camera);
}

int main() 
{
    const Color backgoundColor = GREEN;    

    bool fullscreen = true;
    int screenWidth;
    int screenHeight;

    if(fullscreen) {
        InitWindow(GetScreenWidth(), GetScreenHeight(), "ZulaLords");
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
    }
    else {
        screenWidth = 1920;
        screenHeight = 1080;
        InitWindow(screenWidth, screenHeight, "ZulaLords");
    }

    SetTargetFPS(60);
    
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    camera.offset = Vector2{ (float)screenWidth / 2.0f, (float)screenHeight / 2.0f };
    camera.target = Vector2{ 0.0f, 0.0f };

    int draggedZone = CLEAR;
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

    float timer    = 0.0f;
    float gameTime  = 0.0f;
    int   day       = 1;
    const float dayLength = 120.0f; // seconds per in-game day

    bool run = false;
    bool pause = false;
    bool settings = false;
    bool build = false;
    bool paths = false;
    bool zones = false;
    bool destroy = false;
    bool walls = false;
    bool items = false;
    bool orders = false;
    bool work_zones = false;
    bool no_money = false;
    bool showZones = false;
    bool showOrders = false;
    bool notplacable = false;
    bool job_assign = false;      // Job assignment mode

    int draggedOrder = 0; // 1 = Chop, 2 = Mine

    int mousehold = 0;

    int selected_zone_index = -1;      // Zone currently selected for job assignment
    int selected_job_type = 0;         // Job type being assigned (0=none, 1=farm, 2=storage_haul, 3=construction)
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
        
        if (!run) {
            ClearBackground(Color{22, 40, 22, 255}); // Dark medieval green for main menu

            // Title
            const char* titleText = "ZulaLords";
            int titleSize = 80;
            int titleW = MeasureText(titleText, titleSize);
            DrawText(titleText, screenWidth/2 - titleW/2, screenHeight/2 - 250, titleSize, Color{200, 160, 60, 255});

            // Subtitle
            const char* subText = "Medieval Colony Simulation";
            int subW = MeasureText(subText, 28);
            DrawText(subText, screenWidth/2 - subW/2, screenHeight/2 - 165, 28, Color{180, 140, 80, 200});

            // Controls hint
            const char* hint = "RMB drag: pan camera  |  Scroll: zoom  |  ESC: pause";
            int hintW = MeasureText(hint, 18);
            DrawText(hint, screenWidth/2 - hintW/2, screenHeight/2 + 200, 18, Color{120, 120, 120, 200});

            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, startButtonY + 100, buttonWidth, buttonHeight }, "Start Game") && !settings) {
                run = true;
                // Starting wood stockpile
                for (int i = 0; i < 30; i++) {
                    Materials newMat;
                    newMat.type = WOOD;
                    newMat.x = (float)GetRandomValue(-300, 300);
                    newMat.y = (float)GetRandomValue(-300, 300);
                    newMat.Barva = Color{139, 90, 43, 255};
                    newMat.walking_towards = false;
                    newMat.picked_up = false;
                    newMat.id = itemIDgen++;
                    materials.push_back(newMat);
                }
                // Starting stone stockpile
                for (int i = 0; i < 20; i++) {
                    Materials newMat;
                    newMat.type = STONE;
                    newMat.x = (float)GetRandomValue(-300, 300);
                    newMat.y = (float)GetRandomValue(-300, 300);
                    newMat.Barva = Color{136, 140, 141, 255};
                    newMat.walking_towards = false;
                    newMat.picked_up = false;
                    newMat.id = itemIDgen++;
                    materials.push_back(newMat);
                }
                // Spawn 3 starting colonists
                for (int s = 0; s < 3; s++) {
                    npc1[alive_npc] = NPC();
                    npc1[alive_npc].x = GetRandomValue(-100, 100);
                    npc1[alive_npc].y = GetRandomValue(-100, 100);
                    npc1[alive_npc].speedX = 0;
                    npc1[alive_npc].speedY = 0;
                    npc1[alive_npc].rad    = 15;
                    npc1[alive_npc].work   = NO_WORK;
                    npc1[alive_npc].doing  = NPC_IDLE;
                    npc1[alive_npc].age    = GetRandomValue(18, 40);
                    npc1[alive_npc].clicked = false;
                    npc1[alive_npc].hasahouse = false;
                    npc1[alive_npc].registeredhouse = false;
                    npc1[alive_npc].holdingitem = false;
                    npc1[alive_npc].found_source = false;
                    npc1[alive_npc].itemID = -1;
                    sprintf(npc1[alive_npc].name, "Colonist %d", s + 1);
                    alive_npc++;
                }
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, LoadButtonY + 100, buttonWidth, buttonHeight }, "Load Game") && !settings) {
                ifstream myfile ("save.txt");
                if (myfile.is_open()) {
                    for (int i = 0; i < cells; i++)
                        for (int j = 0; j < cells; j++)
                            grid[i][j].barv = TerrainColors[TERRAIN_BLANK];
                    wallRegistry.clear();
                    int x, y, r, g, b, a;
                    while (myfile >> x >> y >> r >> g >> b >> a) {
                        if (x >= 0 && x < cells && y >= 0 && y < cells) {
                            grid[x][y].barv = Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
                        }
                    }
                    myfile.close();
                    run = true;
                    // Spawn starting colonists after load too
                    for (int s = 0; s < 3 && alive_npc == 0; s++) {
                        npc1[alive_npc] = NPC();
                        npc1[alive_npc].x = GetRandomValue(-100, 100);
                        npc1[alive_npc].y = GetRandomValue(-100, 100);
                        npc1[alive_npc].rad = 15; npc1[alive_npc].work = NO_WORK;
                        npc1[alive_npc].doing = NPC_IDLE; npc1[alive_npc].age = 25;
                        npc1[alive_npc].holdingitem = false; npc1[alive_npc].found_source = false;
                        npc1[alive_npc].itemID = -1;
                        sprintf(npc1[alive_npc].name, "Colonist %d", s + 1);
                        alive_npc++;
                    }
                }
            }
            if(GuiButton((Rectangle){ screenWidth / 2 - buttonWidth / 2, SettingsButtonY + 100, buttonWidth, buttonHeight }, "Settings") && !settings) {
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
                gameTime += GetFrameTime();
                if (gameTime >= dayLength) { gameTime = 0.0f; day++; }

                for(int i = 0; i < alive_npc; i++) {

                    // === HOUSE REGISTRATION ===
                    if (!npc1[i].registeredhouse && npc1[i].hasahouse) {
                        for (auto& zone : LiveZone) {
                            if (zone.capacity > 0 && zone.type == HOUSE_ZONE &&
                                npc1[i].homeX >= zone.startX && npc1[i].homeX <= zone.endX &&
                                npc1[i].homeY >= zone.startY && npc1[i].homeY <= zone.endY) {
                                zone.capacity--;
                                npc1[i].registeredhouse = true;
                                TraceLog(LOG_INFO, "NPC %d moved into house zone %d!", i, zone.zoneIndex);
                            }
                        }
                    }

                    // === NPC MOVEMENT ===
                    npc1[i].NPC_movement(LiveZone, grid);

                    Vector2 npcPos = Vector2{(float)npc1[i].x, (float)npc1[i].y};

                    // ===========================================================
                    // HUNGER AND EATING FIX
                    // ===========================================================
                    if (npc1[i].doing != NPC_HOME)
                        npc1[i].hunger = std::min(100.0f, npc1[i].hunger + GetFrameTime() * 0.3f);

                    // Emergency Eat Override - Drop current task if starving
                    if (npc1[i].hunger >= 75.0f && !npc1[i].holdingitem && npc1[i].doing != NPC_EATING && npc1[i].doing != NPC_HOME) {
                        // Clear pending targets so they aren't permanently locked
                        for (auto& tree : treesRegistry) {
                            if (tree->walking_towards && abs((tree->drawX + GRID_SIZE/2) - npc1[i].destinationX) < 10) tree->walking_towards = false;
                        }
                        for (auto& rock : stoneRegistry) {
                            if (rock->walking_towards && abs((rock->drawX + GRID_SIZE/2) - npc1[i].destinationX) < 10) rock->walking_towards = false;
                        }
                        npc1[i].doing = NPC_IDLE;
                        npc1[i].found_source = false;
                    }

                    // Seek food when idle and hungry enough
                    if (npc1[i].hunger >= 55.0f && npc1[i].doing == NPC_IDLE && !npc1[i].holdingitem && npc1[i].food_id < 0) {
                        int fi = -1; float bfd = -1.0f;
                        for (int q = 0; q < (int)materials.size(); q++) {
                            if (!materials[q].picked_up && !materials[q].walking_towards && materials[q].type == FOOD) {
                                float fd = Vector2Distance(npcPos, Vector2{(float)materials[q].x, (float)materials[q].y});
                                if (bfd < 0 || fd < bfd) { bfd = fd; fi = q; }
                            }
                        }
                        if (fi >= 0) {
                            npc1[i].food_id = materials[fi].id;
                            npc1[i].destinationX = (int)materials[fi].x;
                            npc1[i].destinationY = (int)materials[fi].y;
                            npc1[i].doing = NPC_EATING;
                            materials[fi].walking_towards = true;
                        }
                    }

                    // Reach food and eat it (expanded radius to prevent getting stuck)
                    if (npc1[i].doing == NPC_EATING) {
                        float fd = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (fd < 20.0f) { 
                            for (int q = 0; q < (int)materials.size(); q++) {
                                if (materials[q].id == npc1[i].food_id) { materials.erase(materials.begin() + q); break; }
                            }
                            npc1[i].hunger  = std::max(0.0f, npc1[i].hunger - 85.0f);
                            npc1[i].food_id = -1;
                            npc1[i].doing   = NPC_IDLE;
                        }
                    }

                    // ===========================================================
                    // CONSTRUCTION LOGIC — evaluate globally per NPC
                    // ===========================================================
                    bool constructionPending = false;
                    for (auto* wall : wallRegistry) {
                        if (!wall->built && wall->haveTexture && !wall->walking_towards) {
                            for (auto& mat : materials) {
                                if (!mat.walking_towards && !mat.picked_up && mat.type == wall->madeoutof) {
                                    constructionPending = true; break;
                                }
                            }
                        }
                        if (constructionPending) break;
                    }

                    if (constructionPending && npc1[i].doing == NPC_WORKING && !npc1[i].found_source && !npc1[i].holdingitem) {
                        npc1[i].doing = NPC_IDLE;
                    }

                    if (npc1[i].doing == NPC_IDLE && !npc1[i].holdingitem && npc1[i].hunger < 75.0f) {
                        int neededType = -1;
                        for (auto* wall : wallRegistry) {
                            if (!wall->built && wall->haveTexture && !wall->walking_towards) {
                                bool haveMat = false;
                                for (auto& mat : materials) {
                                    if (!mat.walking_towards && !mat.picked_up && mat.type == wall->madeoutof) {
                                        haveMat = true; break;
                                    }
                                }
                                if (haveMat) { neededType = wall->madeoutof; break; }
                            }
                        }

                        if (neededType >= 0) {
                            int bestIdx = -1;
                            float bestDist = -1.0f;
                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (!materials[ii].walking_towards && !materials[ii].picked_up && materials[ii].type == neededType) {
                                    float d = Vector2Distance(npcPos, Vector2{(float)materials[ii].x, (float)materials[ii].y});
                                    if (bestDist < 0 || d < bestDist) { bestDist = d; bestIdx = ii; }
                                }
                            }
                            if (bestIdx >= 0) {
                                npc1[i].destinationX = (int)materials[bestIdx].x;
                                npc1[i].destinationY = (int)materials[bestIdx].y;
                                npc1[i].itemID       = materials[bestIdx].id;
                                npc1[i].itemType     = materials[bestIdx].type;
                                npc1[i].doing        = NPC_GATHERING;
                                materials[bestIdx].walking_towards = true;
                            }
                        }
                    }

                    if (npc1[i].doing == NPC_GATHERING) {
                        float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (d < 15.0f) {
                            for (int ii = 0; ii < (int)materials.size(); ii++) {
                                if (materials[ii].id == npc1[i].itemID) {
                                    materials[ii].picked_up = true;
                                    npc1[i].holdingitem = true;
                                    npc1[i].doing = NPC_IDLE;
                                    break;
                                }
                            }
                        }
                    }

                    if (npc1[i].doing == NPC_IDLE && npc1[i].holdingitem) {
                        Object* targetWall = nullptr;
                        float bestDist = -1.0f;
                        for (auto* wall : wallRegistry) {
                            if (!wall->built && wall->haveTexture && wall->madeoutof == npc1[i].itemType && !wall->walking_towards) {
                                float d = Vector2Distance(npcPos, Vector2{(float)(wall->drawX + GRID_SIZE/2), (float)(wall->drawY + GRID_SIZE/2)});
                                if (bestDist < 0 || d < bestDist) { bestDist = d; targetWall = wall; }
                            }
                        }
                        if (targetWall) {
                            npc1[i].destinationX = targetWall->drawX + GRID_SIZE/2;
                            npc1[i].destinationY = targetWall->drawY + GRID_SIZE/2;
                            npc1[i].doing = NPC_BUILDING;
                            targetWall->walking_towards = true;
                        } else {
                            // No walls, wait a bit or drop item (logic to drop handled below if farm delivery triggers)
                            bool shouldDrop = true;
                            // Checking if it's food delivery instead
                            if (npc1[i].itemType == FOOD) shouldDrop = false; 
                            
                            if (shouldDrop) {
                                for (int m = 0; m < (int)materials.size(); m++) {
                                    if (materials[m].id == npc1[i].itemID) {
                                        materials[m].picked_up = false;
                                        materials[m].walking_towards = false;
                                        break;
                                    }
                                }
                                npc1[i].holdingitem = false;
                            }
                        }
                    }

                    if (npc1[i].doing == NPC_BUILDING) {
                        float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (d < 18.0f) {
                            bool built = false;
                            for (auto* wall : wallRegistry) {
                                if (!wall->built && wall->haveTexture &&
                                    abs((wall->drawX + GRID_SIZE/2) - npc1[i].destinationX) < GRID_SIZE/2 &&
                                    abs((wall->drawY + GRID_SIZE/2) - npc1[i].destinationY) < GRID_SIZE/2) {
                                    for (int m = 0; m < (int)materials.size(); m++) {
                                        if (materials[m].id == npc1[i].itemID) {
                                            materials.erase(materials.begin() + m);
                                            break;
                                        }
                                    }
                                    wall->built = true; wall->walking_towards = false;
                                    npc1[i].holdingitem = false; npc1[i].doing = NPC_IDLE; built = true;
                                    break;
                                }
                            }
                            if (!built) {
                                for (auto* wall : wallRegistry) {
                                    if (!wall->built && abs((wall->drawX + GRID_SIZE/2) - npc1[i].destinationX) < GRID_SIZE/2) {
                                        wall->walking_towards = false;
                                    }
                                }
                                npc1[i].doing = NPC_IDLE;
                            }
                        }
                    }

                    // ===========================================================
                    // ORDERS SYSTEM — Process designated CHOP / MINE commands
                    // ===========================================================
                    if (npc1[i].doing == NPC_IDLE && !npc1[i].holdingitem && npc1[i].hunger < 75.0f && !constructionPending) {
                        Object* targetJob = nullptr;
                        float bestDist = -1.0f;
                        int jobType = 0; // 1 = chop, 2 = mine

                        // Find designated trees
                        for (auto& tree : treesRegistry) {
                            if (tree->designated_chop && !tree->walking_towards) {
                                float d = Vector2Distance(npcPos, Vector2{(float)(tree->drawX + GRID_SIZE/2), (float)(tree->drawY + GRID_SIZE/2)});
                                if (bestDist < 0 || d < bestDist) { bestDist = d; targetJob = tree; jobType = 1; }
                            }
                        }
                        // Find designated rocks
                        for (auto& rock : stoneRegistry) {
                            if (rock->designated_mine && !rock->walking_towards) {
                                float d = Vector2Distance(npcPos, Vector2{(float)(rock->drawX + GRID_SIZE/2), (float)(rock->drawY + GRID_SIZE/2)});
                                if (bestDist < 0 || d < bestDist) { bestDist = d; targetJob = rock; jobType = 2; }
                            }
                        }

                        if (targetJob) {
                            targetJob->walking_towards = true;
                            npc1[i].destinationX = targetJob->drawX + GRID_SIZE/2;
                            npc1[i].destinationY = targetJob->drawY + GRID_SIZE/2;
                            npc1[i].found_source = true;
                            npc1[i].doing = (jobType == 1) ? NPC_LOGGING : NPC_MINING;
                        }
                    }

                    // CHOPPING EXECUTION
                    if (npc1[i].doing == NPC_LOGGING && !npc1[i].holdingitem) {
                        float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (d < (float)GRID_SIZE) {
                            bool chopped = false;
                            for (auto it = treesRegistry.begin(); it != treesRegistry.end(); ) {
                                Object* tree = *it;
                                int cx = tree->drawX + GRID_SIZE/2; int cy = tree->drawY + GRID_SIZE/2;
                                if (abs(cx - npc1[i].destinationX) < GRID_SIZE/2 && abs(cy - npc1[i].destinationY) < GRID_SIZE/2) {
                                    tree->barv = Color{0,0,0,0};
                                    tree->materialsource = NO_SOURCE;
                                    tree->designated_chop = false;
                                    tree->walking_towards = false;
                                    it = treesRegistry.erase(it);

                                    Materials newMat;
                                    newMat.type = WOOD; newMat.x = (float)npc1[i].x; newMat.y = (float)npc1[i].y;
                                    newMat.Barva = Color{139, 90, 43, 255}; newMat.walking_towards = true;
                                    newMat.picked_up = true; newMat.id = itemIDgen++;
                                    materials.push_back(newMat);

                                    npc1[i].holdingitem = true; npc1[i].itemID = newMat.id; npc1[i].itemType = WOOD;
                                    npc1[i].found_source = false;

                                    // Send to nearest storage
                                    int destX = npc1[i].x, destY = npc1[i].y;
                                    float bestSD = -1.0f;
                                    for (auto& sz : LiveZone) {
                                        if (sz.type == STORAGE_ZONE && !sz.ownedCells.empty()) {
                                            float sx=0,sy=0; for(auto& sp: sz.ownedCells){sx+=sp.x;sy+=sp.y;}
                                            float scx=(sx/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                            float scy=(sy/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                            float sd=Vector2Distance(npcPos, Vector2{scx,scy});
                                            if(bestSD<0||sd<bestSD){bestSD=sd;destX=(int)scx;destY=(int)scy;}
                                        }
                                    }
                                    npc1[i].destinationX = destX; npc1[i].destinationY = destY;
                                    chopped = true; break;
                                } else { ++it; }
                            }
                            if (!chopped) npc1[i].doing = NPC_IDLE; 
                        }
                    }

                    // MINING EXECUTION
                    if (npc1[i].doing == NPC_MINING && !npc1[i].holdingitem) {
                        float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (d < (float)GRID_SIZE) {
                            bool mined = false;
                            for (auto it = stoneRegistry.begin(); it != stoneRegistry.end(); ) {
                                Object* rock = *it;
                                int cx = rock->drawX + GRID_SIZE/2; int cy = rock->drawY + GRID_SIZE/2;
                                if (abs(cx - npc1[i].destinationX) < GRID_SIZE/2 && abs(cy - npc1[i].destinationY) < GRID_SIZE/2) {
                                    rock->barv = Color{0,0,0,0};
                                    rock->materialsource = NO_SOURCE;
                                    rock->designated_mine = false;
                                    rock->walking_towards = false;
                                    it = stoneRegistry.erase(it);

                                    Materials newMat;
                                    newMat.type = STONE; newMat.x = (float)npc1[i].x; newMat.y = (float)npc1[i].y;
                                    newMat.Barva = Color{136, 140, 141, 255}; newMat.walking_towards = true;
                                    newMat.picked_up = true; newMat.id = itemIDgen++;
                                    materials.push_back(newMat);

                                    npc1[i].holdingitem = true; npc1[i].itemID = newMat.id; npc1[i].itemType = STONE;
                                    npc1[i].found_source = false;

                                    // Send to nearest storage
                                    int destX = npc1[i].x, destY = npc1[i].y;
                                    float bestSD = -1.0f;
                                    for (auto& sz : LiveZone) {
                                        if (sz.type == STORAGE_ZONE && !sz.ownedCells.empty()) {
                                            float sx=0,sy=0; for(auto& sp: sz.ownedCells){sx+=sp.x;sy+=sp.y;}
                                            float scx=(sx/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                            float scy=(sy/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                            float sd=Vector2Distance(npcPos, Vector2{scx,scy});
                                            if(bestSD<0||sd<bestSD){bestSD=sd;destX=(int)scx;destY=(int)scy;}
                                        }
                                    }
                                    npc1[i].destinationX = destX; npc1[i].destinationY = destY;
                                    mined = true; break;
                                } else { ++it; }
                            }
                            if (!mined) npc1[i].doing = NPC_IDLE;
                        }
                    }

                    // DELIVERING WOOD OR STONE
                    if (npc1[i].holdingitem && (npc1[i].doing == NPC_LOGGING || npc1[i].doing == NPC_MINING)) {
                        float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                        if (d < (float)GRID_SIZE) {
                            for (int m = 0; m < (int)materials.size(); m++) {
                                if (materials[m].id == npc1[i].itemID) {
                                    materials[m].picked_up = false; materials[m].walking_towards = false;
                                    materials[m].x = (float)npc1[i].destinationX; materials[m].y = (float)npc1[i].destinationY;
                                    break;
                                }
                            }
                            npc1[i].holdingitem = false; npc1[i].itemID = -1;
                            // After delivery, look for new work instead of going idle immediately
                            if (npc1[i].work == FARM && !npc1[i].found_source) {
                                // Still working at farm, stay in WORKING state
                                npc1[i].doing = NPC_WORKING;
                            } else {
                                // No more work here, go home or find new job
                                if (npc1[i].hasahouse) {
                                    npc1[i].doing = NPC_WALKING_TO_HOME;
                                } else {
                                    npc1[i].doing = NPC_IDLE;
                                }
                            }
                        }
                    }

                    // ===========================================================
                    // FARMING — plant, grow, harvest food
                    // ===========================================================
                    if ((npc1[i].doing == NPC_WORKING || npc1[i].doing == NPC_FARMING) && npc1[i].work == FARM) {

                        if (!npc1[i].found_source && !npc1[i].holdingitem) {
                            Object* target = nullptr;
                            bool isHarvest = false;
                            float bestDist = -1.0f;

                            for (auto& zone : LiveZone) {
                                if (zone.type == WORK_FARM_ZONE) {
                                    for (auto& p : zone.ownedCells) {
                                        if (grid[p.x][p.y].walking_towards) continue;
                                        float cx = (float)(grid[p.x][p.y].drawX + GRID_SIZE/2);
                                        float cy = (float)(grid[p.x][p.y].drawY + GRID_SIZE/2);
                                        float d  = Vector2Distance(npcPos, Vector2{cx, cy});
                                        if (grid[p.x][p.y].crop_ready) {
                                            if (!isHarvest || d < bestDist) { bestDist = d; target = &grid[p.x][p.y]; isHarvest = true; }
                                        } else if (!isHarvest && !grid[p.x][p.y].crop_planted) {
                                            if (bestDist < 0 || d < bestDist) { bestDist = d; target = &grid[p.x][p.y]; }
                                        }
                                    }
                                }
                            }

                            if (target) {
                                target->walking_towards = true;
                                npc1[i].destinationX   = target->drawX + GRID_SIZE/2;
                                npc1[i].destinationY   = target->drawY + GRID_SIZE/2;
                                npc1[i].found_source   = true;
                                npc1[i].doing          = NPC_FARMING;
                            } else {
                                if (GetRandomValue(0, 100) < 5) {
                                    npc1[i].destinationX = npc1[i].workplaceX + GetRandomValue(-2*GRID_SIZE, 2*GRID_SIZE);
                                    npc1[i].destinationY = npc1[i].workplaceY + GetRandomValue(-2*GRID_SIZE, 2*GRID_SIZE);
                                }
                                npc1[i].doing = NPC_WORKING;
                            }
                        }

                        if (npc1[i].found_source && !npc1[i].holdingitem) {
                            float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                            if (d < (float)GRID_SIZE) {
                                bool acted = false;
                                for (auto& zone : LiveZone) {
                                    if (acted || zone.type != WORK_FARM_ZONE) continue;
                                    for (auto& p : zone.ownedCells) {
                                        int cx = grid[p.x][p.y].drawX + GRID_SIZE/2;
                                        int cy = grid[p.x][p.y].drawY + GRID_SIZE/2;
                                        if (abs(cx - npc1[i].destinationX) < GRID_SIZE/2 && abs(cy - npc1[i].destinationY) < GRID_SIZE/2) {
                                            grid[p.x][p.y].walking_towards = false;
                                            if (grid[p.x][p.y].crop_ready) {
                                                grid[p.x][p.y].crop_planted = false; grid[p.x][p.y].crop_ready = false; grid[p.x][p.y].crop_timer = 0.0f;
                                                grid[p.x][p.y].barv = Color{101, 67, 33, 200};
                                                Materials food; food.type = FOOD;
                                                food.x = (float)npc1[i].x; food.y = (float)npc1[i].y;
                                                food.Barva = Color{80, 200, 80, 255}; food.walking_towards = true; food.picked_up = true;
                                                food.id = itemIDgen++; materials.push_back(food);
                                                npc1[i].holdingitem = true; npc1[i].itemID = food.id; npc1[i].itemType = FOOD;
                                                npc1[i].found_source = false;
                                                
                                                int destX = npc1[i].workplaceX, destY = npc1[i].workplaceY;
                                                float bestSD = -1.0f;
                                                for (auto& sz : LiveZone) {
                                                    if (sz.type == STORAGE_ZONE && !sz.ownedCells.empty()) {
                                                        float sx=0,sy=0; for (auto& sp : sz.ownedCells){sx+=sp.x;sy+=sp.y;}
                                                        float scx=(sx/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                                        float scy=(sy/sz.ownedCells.size())*GRID_SIZE-gridArea+GRID_SIZE/2;
                                                        float sd=Vector2Distance(npcPos, Vector2{scx,scy});
                                                        if(bestSD<0||sd<bestSD){bestSD=sd;destX=(int)scx;destY=(int)scy;}
                                                    }
                                                }
                                                npc1[i].destinationX=destX; npc1[i].destinationY=destY;
                                            } else if (!grid[p.x][p.y].crop_planted) {
                                                grid[p.x][p.y].crop_planted = true; grid[p.x][p.y].crop_timer = 0.0f;
                                                grid[p.x][p.y].barv = Color{34, 139, 34, 200};
                                                npc1[i].found_source = false; npc1[i].doing = NPC_WORKING;
                                            } else {
                                                npc1[i].found_source = false; npc1[i].doing = NPC_WORKING;
                                            }
                                            acted = true; break;
                                        }
                                    }
                                }
                                if (!acted) { npc1[i].found_source = false; npc1[i].doing = NPC_WORKING; }
                            }
                        }

                        if (npc1[i].holdingitem && npc1[i].doing == NPC_FARMING) {
                            float d = Vector2Distance(npcPos, Vector2{(float)npc1[i].destinationX, (float)npc1[i].destinationY});
                            if (d < (float)GRID_SIZE) {
                                for (int m = 0; m < (int)materials.size(); m++) {
                                    if (materials[m].id == npc1[i].itemID) {
                                        materials[m].picked_up = false; materials[m].walking_towards = false;
                                        materials[m].x = (float)npc1[i].destinationX; materials[m].y = (float)npc1[i].destinationY;
                                        break;
                                    }
                                }
                                npc1[i].holdingitem = false; npc1[i].itemID = -1; npc1[i].doing = NPC_WORKING;
                            }
                        }
                    }

                }
            }

            // ===========================================================
            // CROP GROWTH
            // ===========================================================
            float dt = GetFrameTime();
            for (auto& zone : LiveZone) {
                if (zone.type == WORK_FARM_ZONE) {
                    for (auto& p : zone.ownedCells) {
                        auto& cell = grid[p.x][p.y];
                        const float GROW_TIME = 40.0f;

                        if (!cell.crop_planted && !cell.crop_ready) {
                            cell.barv = Color{101, 67, 33, 220};
                        }

                        if (cell.crop_planted && !cell.crop_ready) {
                            cell.crop_timer += dt;
                            float t = cell.crop_timer / GROW_TIME;
                            if (t > 1.0f) t = 1.0f;

                            unsigned char r, g, b;
                            if (t < 0.35f) {
                                float s = t / 0.35f;
                                r = (unsigned char)(20  + s * 40);
                                g = (unsigned char)(100 + s * 80);
                                b = (unsigned char)(20  + s * 10);
                            } else if (t < 0.7f) {
                                float s = (t - 0.35f) / 0.35f;
                                r = (unsigned char)(60  + s * 150);
                                g = (unsigned char)(180 + s * 30);
                                b = (unsigned char)(30  - s * 20);
                            } else {
                                float s = (t - 0.7f) / 0.3f;
                                r = (unsigned char)(210 + s * 45);
                                g = (unsigned char)(210 + s * 5);
                                b = (unsigned char)(10);
                            }
                            cell.barv = Color{r, g, b, 230};

                            if (cell.crop_timer >= GROW_TIME) {
                                cell.crop_ready = true;
                                cell.barv = Color{255, 215, 0, 240}; 
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
            
            // Draw order designations
            Vector2 BotRig  = Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() };
            Vector2 TopLeft = GetScreenToWorld2D(Vector2{ 0.0f, 0.0f }, camera);
            Vector2 BotRight  = GetScreenToWorld2D(BotRig, camera);
            Vector2 TopLVec = Vector2{(TopLeft.x / GRID_SIZE) * GRID_SIZE, (TopLeft.y / GRID_SIZE) * GRID_SIZE};
            Vector2 BotRVec = Vector2{(BotRight.x / GRID_SIZE) * GRID_SIZE, (BotRight.y / GRID_SIZE) * GRID_SIZE};
            Vector2 TL_drw = Vector2{(TopLVec.x + gridArea) / GRID_SIZE,(TopLVec.y + gridArea) / GRID_SIZE};
            Vector2 BR_drw = Vector2{(BotRVec.x + gridArea) / GRID_SIZE,(BotRVec.y + gridArea) / GRID_SIZE};
            
            if ((int)TL_drw.x < 0) TL_drw.x = 0;
            if ((int)BR_drw.x > cells) BR_drw.x = cells;
            if ((int)TL_drw.y < 0) TL_drw.y = 0;
            if ((int)BR_drw.y > cells) BR_drw.y = cells;

            for (int i = TL_drw.x; i < BR_drw.x; i++) {
                for (int j = TL_drw.y; j < BR_drw.y; j++) {
                    if (grid[i][j].designated_chop) {
                        DrawLineEx(Vector2{(float)grid[i][j].drawX + 15, (float)grid[i][j].drawY + 15}, Vector2{(float)grid[i][j].drawX + GRID_SIZE - 15, (float)grid[i][j].drawY + GRID_SIZE - 15}, 4, RED);
                        DrawLineEx(Vector2{(float)grid[i][j].drawX + GRID_SIZE - 15, (float)grid[i][j].drawY + 15}, Vector2{(float)grid[i][j].drawX + 15, (float)grid[i][j].drawY + GRID_SIZE - 15}, 4, RED);
                    }
                    if (grid[i][j].designated_mine) {
                        DrawLineEx(Vector2{(float)grid[i][j].drawX + 15, (float)grid[i][j].drawY + 15}, Vector2{(float)grid[i][j].drawX + GRID_SIZE - 15, (float)grid[i][j].drawY + GRID_SIZE - 15}, 4, YELLOW);
                        DrawLineEx(Vector2{(float)grid[i][j].drawX + GRID_SIZE - 15, (float)grid[i][j].drawY + 15}, Vector2{(float)grid[i][j].drawX + 15, (float)grid[i][j].drawY + GRID_SIZE - 15}, 4, YELLOW);
                    }
                }
            }

            for(int i = 0; i < alive_npc; i++) {
                npc1[i].draw();
            }

            for (int i = 0; i < (int)materials.size(); i++) {
                if (!npc1.empty() && materials[i].picked_up) {
                    for (int ii = 0; ii < alive_npc; ii++) {
                        if (npc1[ii].holdingitem && npc1[ii].itemID == materials[i].id) {
                            materials[i].x = npc1[ii].x;
                            materials[i].y = npc1[ii].y;
                        }
                    }
                }
                materials[i].Draw();
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

            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !pause && (paths | destroy | walls | zones | orders) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, (float)screenWidth, 50}) && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){100, 130, 100, 140}))
            {
                int StartX = (gridX < StartGridX) ? gridX : StartGridX;
                int EndX = (gridX > StartGridX) ? gridX : StartGridX;
                int StartY = (gridY < StartGridY) ? gridY : StartGridY;
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
                            if (orders && draggedOrder != 0) {
                                if (draggedOrder == 1 && grid[i][ii].materialsource == TREE) {
                                    grid[i][ii].designated_chop = true;
                                }
                                else if (draggedOrder == 2 && grid[i][ii].materialsource == ROCK) {
                                    grid[i][ii].designated_mine = true;
                                }
                            }
                            
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

                        checkNeighbor(p.x, p.y - 1);
                        checkNeighbor(p.x, p.y + 1);
                        checkNeighbor(p.x - 1, p.y);
                        checkNeighbor(p.x + 1, p.y);
                    }

                    if (!zonesToMergeWith.empty()) 
                    {
                        int masterZoneId = zonesToMergeWith[0];

                        for (auto& p : LiveZone[currentZoneIndex].ownedCells) {
                            LiveZone[masterZoneId].ownedCells.push_back(p);
                            grid[p.x][p.y].myzone = masterZoneId;
                        }
                        LiveZone[currentZoneIndex].ownedCells.clear(); 

                        for (size_t k = 1; k < zonesToMergeWith.size(); k++) 
                        {
                            int otherZoneId = zonesToMergeWith[k];
                            for (auto& p : LiveZone[otherZoneId].ownedCells) {
                                LiveZone[masterZoneId].ownedCells.push_back(p);
                                grid[p.x][p.y].myzone = masterZoneId;
                            }
                            LiveZone[otherZoneId].ownedCells.clear(); 
                        }
                    }
                }
            }

            for(int i = 0; i < alive_npc; i++) {
                if ((mouseWorldPos.x <= npc1[i].x + npc1[i].rad && mouseWorldPos.x >= npc1[i].x - npc1[i].rad) && 
                    (mouseWorldPos.y <= npc1[i].y + npc1[i].rad && mouseWorldPos.y >= npc1[i].y - npc1[i].rad) && 
                    IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    // Instant Job Assignment Logic
                    if (job_assign && selected_zone_index >= 0 && selected_job_type > 0) {
                        npc1[i].assigned_zone_id = selected_zone_index;
                        npc1[i].assigned_job_type = selected_job_type;
                        
                        // Force the NPC to restart their behavior loop with the new job immediately
                        npc1[i].doing = NPC_IDLE;
                        npc1[i].found_source = false;

                        // Apply work status and workplace coordinates if a farmer
                        if (selected_job_type == 1) {
                            npc1[i].work = FARM;
                            float sx = 0, sy = 0;
                            for (auto& p : LiveZone[selected_zone_index].ownedCells) { sx += p.x; sy += p.y; }
                            if (!LiveZone[selected_zone_index].ownedCells.empty()) {
                                npc1[i].workplaceX = (int)((sx / LiveZone[selected_zone_index].ownedCells.size()) * GRID_SIZE - gridArea + GRID_SIZE/2);
                                npc1[i].workplaceY = (int)((sy / LiveZone[selected_zone_index].ownedCells.size()) * GRID_SIZE - gridArea + GRID_SIZE/2);
                            }
                        } else {
                            npc1[i].work = NO_WORK; // Removes their farm assignment if they switch to hauling/etc
                        }
                        
                        TraceLog(LOG_INFO, "Assigned NPC %s to zone %d for job type %d", npc1[i].name, selected_zone_index, selected_job_type);
                    } 
                    // Standard NPC Info Panel Logic
                    else if (!npc1[i].clicked) {
                        npc1[i].clicked = true;
                    } 
                    else {
                        npc1[i].clicked = false;
                    }
                }
            }

            for (int i = 0; i < alive_npc; i++)
            {
                if (npc1[i].hunger >= 80.0f) {
                    float pulse = 0.5f + 0.5f * sinf(GetTime() * 6.0f);
                    Color warnCol = Color{255, (unsigned char)(50 + 50*pulse), 50, 255};
                    DrawText("!", npc1[i].x - 4, npc1[i].y - npc1[i].rad - 22, 20, warnCol);
                }

                if (npc1[i].clicked) {
                    camera.target = Vector2{(float)npc1[i].x, (float)npc1[i].y};

                    int panelX = npc1[i].x - 70;
                    int panelY = npc1[i].y - 140;
                    DrawRectangle(panelX, panelY, 150, 120, Color{40, 30, 20, 210});
                    DrawRectangleLines(panelX, panelY, 150, 120, Color{180,140,60,200});

                    char buf[128];
                    DrawText(npc1[i].name, panelX + 5, panelY + 5, 14, RAYWHITE);
                    sprintf(buf, "Age: %d", npc1[i].age);
                    DrawText(buf, panelX + 5, panelY + 21, 13, Color{200,200,180,255});

                    const char* stateStr = "Idle";
                    switch(npc1[i].doing) {
                        case NPC_WALKING_TO_WORK: stateStr = "Walking to work"; break;
                        case NPC_WORKING:  stateStr = "Working"; break;
                        case NPC_LOGGING:  stateStr = "Chopping wood"; break;
                        case NPC_MINING:   stateStr = "Mining stone"; break;
                        case NPC_FARMING:  stateStr = "Farming"; break;
                        case NPC_BUILDING: stateStr = "Building"; break;
                        case NPC_GATHERING:stateStr = "Gathering materials"; break;
                        case NPC_EATING:   stateStr = "Eating"; break;
                        case NPC_HOME:     stateStr = "Resting"; break;
                        default: break;
                    }
                    DrawText(stateStr, panelX + 5, panelY + 37, 12, Color{180,220,180,255});

                    const char* workStr = "No job";
                    if (npc1[i].work == FARM)   workStr = "Job: Farmer";
                    DrawText(workStr, panelX + 5, panelY + 52, 12, Color{180,200,255,255});

                    DrawText("Hunger:", panelX + 5, panelY + 69, 12, Color{200,200,200,255});
                    int barW = 100, barH = 10;
                    DrawRectangle(panelX + 5, panelY + 83, barW, barH, Color{60,60,60,200});
                    
                    // Reversed hunger bar: 1.0f means full width (not starving), 0.0f means empty (starving)
                    float hFrac = 1.0f - (npc1[i].hunger / 100.0f);
                    if (hFrac < 0.0f) hFrac = 0.0f;
                    if (hFrac > 1.0f) hFrac = 1.0f;
                    
                    unsigned char hR = (unsigned char)(255 - 200 * hFrac);
                    unsigned char hG = (unsigned char)(50 + 150 * hFrac);
                    
                    DrawRectangle(panelX + 5, panelY + 83, (int)(barW*hFrac), barH, Color{hR, hG, 50, 230});
                    DrawRectangleLines(panelX + 5, panelY + 83, barW, barH, Color{180,180,180,150});

                    if (npc1[i].holdingitem) {
                        const char* item = (npc1[i].itemType == WOOD) ? "[Wood]" :
                                           (npc1[i].itemType == STONE) ? "[Stone]" :
                                           (npc1[i].itemType == FOOD)  ? "[Food]" : "[?]";
                        DrawText(item, panelX + 5, panelY + 97, 13, Color{255,215,0,255});
                    }
                }
            }
            
            for (auto& zone : LiveZone) {
                zone.CheckValidity(grid);
                // Temporarily force zones to show while actively assigning jobs
                if (showZones || job_assign || !zone.valid)
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

            int woodCount = 0, stoneCount = 0;
            for (auto& m : materials) { if (!m.picked_up) { if (m.type == WOOD) woodCount++; else if (m.type == STONE) stoneCount++; } }

            char hudBuf[128];
            sprintf(hudBuf, "Day %d", day);
            DrawText(hudBuf, screenWidth/2 - 40, 10, 28, RAYWHITE);

            sprintf(hudBuf, "Wood: %d", woodCount);
            DrawText(hudBuf, 500, 12, 22, Color{139, 90, 43, 255});

            sprintf(hudBuf, "Stone: %d", stoneCount);
            DrawText(hudBuf, 650, 12, 22, Color{136, 140, 141, 255});

            int foodCount = 0;
            for (auto& m : materials) { if (!m.picked_up && m.type == FOOD) foodCount++; }
            sprintf(hudBuf, "Food: %d", foodCount);
            DrawText(hudBuf, 800, 12, 22, Color{80, 200, 80, 255});

            GuiValueBox((Rectangle){990, 0, 100, 50}, "People", &alive_npc, 0, 10000, false);

            if (GuiButton((Rectangle){0, fromtop, 100, 50}, "PATHS") && !build){
                paths = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, "ZONES") && !build){
                zones = true;
                build = true;
                showZones = true;
            }
            if (GuiButton((Rectangle){0, fromtop+100, 100, 50}, "ORDERS") && !build){
                build = true;
                orders = true;
                showOrders = true;
            }
            if (GuiButton((Rectangle){0, fromtop+150, 100, 50}, "WALLS") && !build){
                build = true;
                walls = true;
            }

            if (GuiButton((Rectangle){0, fromtop+200, 100, 50}, "DESTROY") && !build){
                destroy = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+250, 100, 50}, "ITEMS") && !build){
                items = true;
                build = true;
            }
            if (GuiButton((Rectangle){0, fromtop+300, 100, 50}, "+ Colonist") && !pause){
                npc1[alive_npc] = NPC();
                npc1[alive_npc].x = 0;
                npc1[alive_npc].y = 0;
                npc1[alive_npc].speedX = 0;
                npc1[alive_npc].speedY = 0;
                npc1[alive_npc].rad = 15;
                npc1[alive_npc].work = NO_WORK;
                npc1[alive_npc].doing = NPC_IDLE;
                npc1[alive_npc].age = GetRandomValue(18, 40);
                npc1[alive_npc].clicked = false;
                npc1[alive_npc].hasahouse = false;
                npc1[alive_npc].registeredhouse = false;
                npc1[alive_npc].holdingitem = false;
                npc1[alive_npc].found_source = false;
                npc1[alive_npc].itemID = -1;
                sprintf(npc1[alive_npc].name, "Colonist %d", alive_npc + 1);
                alive_npc++;
            }

            // Job Assignment Menu - accessible from main menu
            if (GuiButton((Rectangle){0, fromtop+350, 100, 50}, "JOB ASSIGN") && !build) {
                job_assign = true;
                build = true;
                selected_zone_index = -1;
                selected_job_type = 0;
            }

            if (GuiButton((Rectangle){(float)(screenWidth - 150), 0, 100.0f, 50.0f}, "Show zones") && !pause){
                showZones = !showZones;
            }
            if (GuiButton((Rectangle){(float)(screenWidth - 150), 50.0f, 100.0f, 50.0f}, "Show orders") && !pause){
                showOrders = !showOrders;
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
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "FARM ZONE")){
                        draggedZone = WORK_FARM_ZONE;
                    }
                    if (GuiButton((Rectangle){0, 3*fromtop, 100, 50}, "STORAGE")){
                        draggedZone = STORAGE_ZONE;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "CLEAR ZONES")){
                        draggedZone = CLEAR;
                    }
                    if (GuiButton((Rectangle){0, 5*fromtop, 100, 50}, "CANCEL")){
                        zones = false;
                        build = false;
                        showZones = false;
                    }
                }

                if (orders) {
                    DrawText("Select order type", 20, fromtop - 10, 20, GRAY);
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "CHOP TREES")){
                        draggedOrder = 1;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "MINE ROCKS")){
                        draggedOrder = 2;
                    }
                    if (GuiButton((Rectangle){0, 4*fromtop, 100, 50}, "CANCEL")){ 
                        orders = false;
                        build = false;
                        showOrders = false;
                        draggedOrder = 0;
                    }
                }

                if (walls) {
                    if (GuiButton((Rectangle){0, fromtop, 100, 50}, "Stone wall")){
                        mousehold = Wall_Stone;
                    }
                    if (GuiButton((Rectangle){0, 2*fromtop, 100, 50}, "Wooden wall")){
                        mousehold = Wall_Wooden;
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

                if (items) {
                    DrawRectangle(100, fromtop-5, 110, 160, Color(BROWN));
                    if (GuiButton((Rectangle){100, fromtop, 100, 50}, "BED")){
                        mousehold = -ITEM_BED; 
                    }
                    if (GuiButton((Rectangle){100, 2*fromtop, 100, 50}, "DOOR")){
                        mousehold = -ITEM_DOOR;
                    }
                    if (GuiButton((Rectangle){100, 3*fromtop, 100, 50}, "CLEAR")){
                        mousehold = -ITEM_NONE;
                    }
                    if (GuiButton((Rectangle){100, 4*fromtop-10, 100, 40}, "CANCEL")){
                        items = false;
                        build = false;
                        mousehold = 0;
                    }
                }
                
                // --- REWRITTEN JOB ASSIGNMENT FLOW ---
                if (job_assign) {
                    // Draw a background panel to host the instructions
                    DrawRectangle(100, fromtop, 210, 160, Fade(BROWN, 0.9f));
                    DrawRectangleLines(100, fromtop, 210, 160, DARKBROWN);

                    if (selected_zone_index == -1) {
                        // STATE 1: Waiting to select a zone
                        DrawText("JOB ASSIGNMENT", 110, fromtop + 10, 20, GOLD);
                        DrawText("Step 1:", 110, fromtop + 40, 16, WHITE);
                        DrawText("Click a zone", 110, fromtop + 60, 16, LIGHTGRAY);
                        DrawText("on the map.", 110, fromtop + 80, 16, LIGHTGRAY);
                        
                        if (GuiButton((Rectangle){0, fromtop, 100, 50}, "CANCEL")) {
                            job_assign = false; build = false;
                        }
                        
                        int assignedCount = 0;
                        for(int i=0; i<alive_npc; i++) if(npc1[i].assigned_zone_id >= 0) assignedCount++;
                        DrawText(TextFormat("Total Assigned: %d/%d", assignedCount, alive_npc), 110, fromtop + 130, 14, GRAY);
                        
                    } else if (selected_job_type == 0) {
                        // STATE 2: Zone selected, picking job type
                        DrawText(TextFormat("Zone %d Selected", selected_zone_index), 110, fromtop + 10, 20, GOLD);
                        DrawText("Step 2:", 110, fromtop + 40, 16, WHITE);
                        DrawText("Select Job Type", 110, fromtop + 60, 16, LIGHTGRAY);
                        
                        if (GuiButton((Rectangle){0, fromtop, 100, 50}, "FARMER")) selected_job_type = 1;
                        if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, "HAULER")) selected_job_type = 2;
                        if (GuiButton((Rectangle){0, fromtop+100, 100, 50}, "BACK")) selected_zone_index = -1;
                        
                    } else {
                        // STATE 3: Ready to assign NPC
                        const char* jobName = (selected_job_type == 1) ? "FARMER" : "HAULER";
                        DrawText(TextFormat("Assign: %s", jobName), 110, fromtop + 10, 20, GOLD);
                        DrawText("Step 3:", 110, fromtop + 40, 16, WHITE);
                        DrawText("Click NPCs on the", 110, fromtop + 60, 16, GREEN);
                        DrawText("map to assign!", 110, fromtop + 80, 16, GREEN);
                        
                        if (GuiButton((Rectangle){0, fromtop, 100, 50}, "BACK")) selected_job_type = 0;
                        if (GuiButton((Rectangle){0, fromtop+50, 100, 50}, "DONE")) {
                            job_assign = false; build = false;
                            selected_zone_index = -1; selected_job_type = 0;
                        }
                    }
                }
                
                // Safely click zones without clicking through the UI menu
                if (job_assign && selected_zone_index == -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
                    !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 310, (float)screenHeight}) && 
                    !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, (float)screenWidth, 50})) {
                    
                    float mx = mouseWorldPos.x, my = mouseWorldPos.y;
                    int gi = (int)((mx + gridArea) / GRID_SIZE);
                    int gj = (int)((my + gridArea) / GRID_SIZE);
                    
                    for (auto& zone : LiveZone) {
                        if (zone.valid && !zone.ownedCells.empty()) {
                            bool inZone = false;
                            for (const auto& cell : zone.ownedCells) {
                                if (cell.x == gi && cell.y == gj) {
                                    inZone = true;
                                    break;
                                }
                            }
                            if (inZone) {
                                selected_zone_index = zone.zoneIndex;
                                TraceLog(LOG_INFO, "Selected zone %d for job assignment", selected_zone_index);
                                break;
                            }
                        }
                    }
                }
                
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mousehold < 0 && 
                    !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, 100, (float)screenHeight}) && 
                    !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, (float)screenWidth, 50})) {
                    
                    float mx = mouseWorldPos.x, my = mouseWorldPos.y;
                    int gi = (int)((mx + gridArea) / GRID_SIZE);
                    int gj = (int)((my + gridArea) / GRID_SIZE);
                    if (gi >= 0 && gi < cells && gj >= 0 && gj < cells) {
                        int itype = -mousehold;
                        grid[gi][gj].has_item  = (itype != ITEM_NONE);
                        grid[gi][gj].item_type = itype;
                        if (itype == ITEM_BED && grid[gi][gj].am_I_zone) {
                            for (auto& zone : LiveZone) {
                                if (zone.zoneIndex == grid[gi][gj].myzone && zone.type == HOUSE_ZONE) {
                                    zone.capacity += 2;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_ESCAPE))
            {
                paths = false;
                zones = false;
                orders = false;
                destroy = false;
                items = false;
                build = false;
                walls = false;
                showZones = false;
                showOrders = false;
                no_money = false;
                notplacable = false;
                work_zones = false;
                job_assign = false;
                selected_zone_index = -1;
                selected_job_type = 0;
                mousehold = 0;
                draggedOrder = 0;
            }
        }
        
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
                            if (grid[i][j].barv.a != 0) { 
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
            if (GuiButton((Rectangle){ pausemenuX + pausemenuW /6, pausemenuY + 3*buttonHeight + 2*pausemenuspacing, buttonWidth, buttonHeight }, "Load Game")) {
                ifstream myfile ("save.txt");
                if (myfile.is_open()) {
                    for (int i = 0; i < cells; i++)
                        for (int j = 0; j < cells; j++)
                            grid[i][j].barv = TerrainColors[TERRAIN_BLANK];
                    wallRegistry.clear();
                    int x, y, r, g, b, a;
                    while (myfile >> x >> y >> r >> g >> b >> a) {
                        if (x >= 0 && x < cells && y >= 0 && y < cells) {
                            grid[x][y].barv = Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
                        }
                    }
                    myfile.close();
                    pause = false;
                }
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
        
        DrawFPS(10, 10);
        EndDrawing();           
    }
    
    CloseWindow();
    return 0;
}