#include "buildings.h"
#include "raymath.h"

void ZoneTemplate::CheckValidity (const std::vector<std::vector<Object>>& grid) {
    valid = true;
    for (auto& point : ownedCells) {
        
        auto checkNeighbor = [&](int nx, int ny) {

            if (nx < 0 || nx >= cells || ny < 0 || ny >= cells) return false;

            if (grid[nx][ny].am_I_zone && grid[nx][ny].myzone == this->zoneIndex) return true;

            if (grid[nx][ny].haveTexture && grid[nx][ny].built) return true;

            // Door items count as valid borders (passable doorway)
            if (grid[nx][ny].has_item && grid[nx][ny].item_type == ITEM_DOOR) return true;

            return false;
        };

        if (type == STORAGE_ZONE || type == WORK_FARM_ZONE) {
            valid = true;
            return;
        }

        if (!checkNeighbor(point.x, point.y - 1) || 
            !checkNeighbor(point.x, point.y + 1) || 
            !checkNeighbor(point.x - 1, point.y) || 
            !checkNeighbor(point.x + 1, point.y)) 
        {
            valid = false;
            break;
        }
    }
}

void ZoneTemplate::draw(Camera2D camera, const std::vector<std::vector<Object>>& grid)
{
    // Draw only cells visible on screen
    Vector2 BotRig  = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    Vector2 TopLeft = GetScreenToWorld2D({ 0.0f, 0.0f }, camera);
    Vector2 BotRight  = GetScreenToWorld2D(BotRig, camera);

    Vector2 TopLVec = (TopLeft / GRID_SIZE) * GRID_SIZE;
    Vector2 BotRVec = (BotRight / GRID_SIZE) * GRID_SIZE;

    Vector2 TL_drw = {(TopLVec.x + gridArea) / GRID_SIZE,(TopLVec.y + gridArea) / GRID_SIZE};
    Vector2 BR_drw = {(BotRVec.x + gridArea) / GRID_SIZE,(BotRVec.y + gridArea) / GRID_SIZE};

    Vector2 mik = {1,1};
    TL_drw -=  mik;
    BR_drw +=  mik;

    if ((int)TL_drw.x < 0) TL_drw.x = 0;
    if ((int)BR_drw.x > cells) BR_drw.x = cells;
    if ((int)TL_drw.y < 0) TL_drw.y = 0;
    if ((int)BR_drw.y > cells) BR_drw.y = cells;  

    auto isSameZone = [&](int cx, int cy) {
        if (cx < 0 || cx >= cells || cy < 0 || cy >= cells) return false;
        return grid[cx][cy].am_I_zone && grid[cx][cy].what_am_I == this->type;
    };

    // Zone fill colors
    Color fillColor = Fade(BLUE, 0.25f);
    if (who_am_I == WORK_FARM_ZONE)    fillColor = Fade(GREEN,  0.30f);
    if (who_am_I == STORAGE_ZONE)      fillColor = Fade({200, 160, 50, 255}, 0.35f);

    Color outlineColor = (valid) ? DARKBLUE : RED;
    if (who_am_I == WORK_FARM_ZONE)    outlineColor = DARKGREEN;
    if (who_am_I == STORAGE_ZONE)      outlineColor = {180, 130, 20, 255};
    float outlineThickness = 3.0f;

    float sumX = 0, sumY = 0;

    for (auto& point : ownedCells) 
    {
        sumX += point.x;
        sumY += point.y;
        if (point.x >= TL_drw.x && point.x <= BR_drw.x && point.y >= TL_drw.y && point.y <= BR_drw.y)
        {
            if (grid[point.x][point.y].am_I_zone == true && grid[point.x][point.y].myzone == this->zoneIndex) {
                int DrawX = (point.x * GRID_SIZE) - gridArea;
                int DrawY = (point.y * GRID_SIZE) - gridArea;

                DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, fillColor);

                // Draw bed icon if item is placed
                if (grid[point.x][point.y].has_item && grid[point.x][point.y].item_type == ITEM_BED) {
                    DrawRectangle(DrawX+8, DrawY+12, GRID_SIZE-16, GRID_SIZE-24, {139,90,43,200});
                    DrawRectangle(DrawX+8, DrawY+10, GRID_SIZE-16, 12, {200,180,150,200});
                    DrawText("z", DrawX + GRID_SIZE/2-4, DrawY+10, 14, {80,60,40,180});
                }

                // Draw door indicator
                if (grid[point.x][point.y].has_item && grid[point.x][point.y].item_type == ITEM_DOOR) {
                    DrawRectangle(DrawX + GRID_SIZE/4, DrawY, GRID_SIZE/2, GRID_SIZE, {180,130,70,200});
                    DrawText("D", DrawX + GRID_SIZE/2-5, DrawY+15, 18, WHITE);
                }

                if (valid) {
                    if (!isSameZone(point.x, point.y-1) && grid[point.x][point.y-1].haveTexture)
                        DrawLineEx({(float)DrawX,(float)DrawY},{(float)(DrawX+GRID_SIZE),(float)DrawY}, outlineThickness, outlineColor);
                    if (!isSameZone(point.x, point.y+1) && grid[point.x][point.y+1].haveTexture)
                        DrawLineEx({(float)DrawX,(float)(DrawY+GRID_SIZE)},{(float)(DrawX+GRID_SIZE),(float)(DrawY+GRID_SIZE)}, outlineThickness, outlineColor);
                    if (!isSameZone(point.x-1, point.y) && grid[point.x-1][point.y].haveTexture)
                        DrawLineEx({(float)DrawX,(float)DrawY},{(float)DrawX,(float)(DrawY+GRID_SIZE)}, outlineThickness, outlineColor);
                    if (!isSameZone(point.x+1, point.y) && grid[point.x+1][point.y].haveTexture)
                        DrawLineEx({(float)(DrawX+GRID_SIZE),(float)DrawY},{(float)(DrawX+GRID_SIZE),(float)(DrawY+GRID_SIZE)}, outlineThickness, outlineColor);
                } else {
                    if (!isSameZone(point.x, point.y-1))
                        DrawLineEx({(float)DrawX,(float)DrawY},{(float)(DrawX+GRID_SIZE),(float)DrawY}, outlineThickness, RED);
                    if (!isSameZone(point.x, point.y+1))
                        DrawLineEx({(float)DrawX,(float)(DrawY+GRID_SIZE)},{(float)(DrawX+GRID_SIZE),(float)(DrawY+GRID_SIZE)}, outlineThickness, RED);
                    if (!isSameZone(point.x-1, point.y))
                        DrawLineEx({(float)DrawX,(float)DrawY},{(float)DrawX,(float)(DrawY+GRID_SIZE)}, outlineThickness, RED);
                    if (!isSameZone(point.x+1, point.y))
                        DrawLineEx({(float)(DrawX+GRID_SIZE),(float)DrawY},{(float)(DrawX+GRID_SIZE),(float)(DrawY+GRID_SIZE)}, outlineThickness, RED);
                }
            }
        }        
    }

    if (!ownedCells.empty()) {
        int centerX = (int)((sumX / ownedCells.size()) * GRID_SIZE) - gridArea;
        int centerY = (int)((sumY / ownedCells.size()) * GRID_SIZE) - gridArea;
        char num[100];
        sprintf(num, "%d", zoneIndex);

        const char* zoneName = "";
        if (who_am_I == HOUSE_ZONE)         zoneName = "House";
        else if (who_am_I == WORK_FARM_ZONE)    zoneName = "Farm";
        else if (who_am_I == STORAGE_ZONE)      zoneName = "Storage";

        DrawText(zoneName, centerX - 15, centerY,    14, WHITE);
        DrawText(num,      centerX + 20, centerY+16, 12, WHITE);

        if (!valid && type != STORAGE_ZONE && type != WORK_FARM_ZONE) {
            DrawText("Needs walls!", centerX-35, centerY+28, 13, RED);
        }
    }

    return;
}

void Object::draw()
{
    if(barv != TerrainColors[TERRAIN_BLANK]) {
        DrawRectangle(drawX, drawY, GRID_SIZE, GRID_SIZE, barv);
    }    
    return;
}

void Object::drawTextures(Texture2D whatTexture)
{
    if (haveTexture && built)
    {
        DrawTexture(whatTexture, drawX,drawY, WHITE);    
    }
    else if (!built) {
        DrawTexture(whatTexture, drawX,drawY, Fade(WHITE, 0.8));
    }
    return;
}