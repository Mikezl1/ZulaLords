#include "buildings.h"
#include "raymath.h"

void ZoneTemplate::CheckValidity (const std::vector<std::vector<Object>>& grid) {
    valid = true;
    for (auto& point : ownedCells) {
        auto checkWall = [&](int nx, int ny) {
            if (nx < 0 || nx >= cells || ny < 0 || ny >= cells) return false;

            if (grid[nx][ny].am_I_zone && grid[nx][ny].myzone == this->zoneIndex && grid[nx][ny].built) return true;

            return grid[nx][ny].haveTexture;
        };

        if (!checkWall(point.x, point.y - 1)) valid = false;
        if (!checkWall(point.x, point.y + 1)) valid = false;
        if (!checkWall(point.x - 1, point.y)) valid = false;
        if (!checkWall(point.x + 1, point.y)) valid = false;

        if (!valid) break;
    }
}

void ZoneTemplate::draw(Camera2D camera, const std::vector<std::vector<Object>>& grid)
{
    // vykresleni jenom bunky ktere jsou vidět na obrazovce
    Vector2 BotRig  = { (float)GetScreenWidth(), (float)GetScreenHeight() };

    Vector2 TopLeft = GetScreenToWorld2D({ 0.0f, 0.0f }, camera);
    Vector2 BotRight  = GetScreenToWorld2D(BotRig, camera);

    Vector2 TopLVec = (TopLeft / GRID_SIZE) * GRID_SIZE;
    Vector2 BotRVec = (BotRight / GRID_SIZE) * GRID_SIZE;

    Vector2 TL_drw = {(TopLVec.x + gridArea) / GRID_SIZE,(TopLVec.y + gridArea) / GRID_SIZE};
    Vector2 BR_drw = {(BotRVec.x + gridArea) / GRID_SIZE,(BotRVec.y + gridArea) / GRID_SIZE};

    Vector2 mik = {1,1};
    TL_drw -=  mik;//mik mik
    BR_drw +=  mik;

    //aby se neprintovaly neexistujici bunky
    if ((int)TL_drw.x < 0) TL_drw.x = 0;

    if ((int)BR_drw.x > cells) BR_drw.x = cells;

    if ((int)TL_drw.y < 0) TL_drw.y = 0;
    
    if ((int)BR_drw.y > cells) BR_drw.y = cells;  

    auto isSameZone = [&](int cx, int cy) {
        if (cx < 0 || cx >= cells || cy < 0 || cy >= cells) return false;
        return grid[cx][cy].am_I_zone && grid[cx][cy].what_am_I == this->type;
    };

    Color outlineColor = (who_am_I == HOUSE_ZONE) ? DARKBLUE : MAROON;
    float outlineThickness = 3.0f;

    if (valid) {
        float sumX = 0, sumY = 0;
        for (auto& point : ownedCells) 
        {
            sumX += point.x;
            sumY += point.y;
            if(point.x >=TL_drw.x && point.x <=BR_drw.x  && point.y >=TL_drw.y && point.y <=BR_drw.y )
            {
                if (grid[point.x][point.y].am_I_zone == true && grid[point.x][point.y].myzone == this->zoneIndex) {
                    int DrawX = (point.x * GRID_SIZE) - gridArea;
                    int DrawY = (point.y * GRID_SIZE) - gridArea;

                    if (!isSameZone(point.x, point.y - 1) && grid[point.x][point.y - 1].haveTexture == true) {
                        DrawLineEx({(float)DrawX, (float)DrawY}, {(float)(DrawX + GRID_SIZE), (float)DrawY}, outlineThickness, outlineColor);
                    }

                    if (!isSameZone(point.x, point.y + 1) && grid[point.x][point.y + 1].haveTexture == true) {
                        DrawLineEx({(float)DrawX, (float)(DrawY + GRID_SIZE)}, {(float)(DrawX + GRID_SIZE), (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
                    }

                    if (!isSameZone(point.x - 1, point.y) && grid[point.x - 1][point.y].haveTexture == true) {
                        DrawLineEx({(float)DrawX, (float)DrawY}, {(float)DrawX, (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
                    }
                    
                    if (!isSameZone(point.x + 1, point.y) && grid[point.x + 1][point.y].haveTexture == true) {
                        DrawLineEx({(float)(DrawX + GRID_SIZE), (float)DrawY}, {(float)(DrawX + GRID_SIZE), (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
                    }
                        
                    if (who_am_I == HOUSE_ZONE) {
                        DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));
                    }
                    else if (who_am_I == SHOP_ZONE) {
                        DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, Fade(RED, 0.3f));
                    }
                    else if (who_am_I == WORK_ZONE) {
                        DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, Fade(YELLOW, 0.3f));
                    }
                }
            }        
        }
        int centerX = (int)((sumX / ownedCells.size()) * GRID_SIZE) - gridArea;
        int centerY = (int)((sumY / ownedCells.size()) * GRID_SIZE) - gridArea;
        char num[100];
        sprintf(num, "%d", zoneIndex);

        if (who_am_I == HOUSE_ZONE) {
            DrawText("House Zone", centerX - 10, centerY, 15, WHITE);
            DrawText(num, centerX + 25, centerY + 20, 15, WHITE);
        }
        else if (who_am_I == SHOP_ZONE) {
            DrawText("Shop Zone", centerX - 10, centerY, 15, WHITE); 
            DrawText(num, centerX + 25, centerY + 20, 15, WHITE);   
        }
        else if (who_am_I == WORK_ZONE) {
            DrawText("Work Zone", centerX - 10, centerY, 15, WHITE);
            DrawText(num, centerX + 25, centerY + 20, 15, WHITE);
        }
    }
    else {
        float sumX = 0, sumY = 0;
        outlineColor = RED;
        valid = false;

        for (const auto& point : ownedCells) {
            sumX += point.x;
            sumY += point.y;

            int DrawX = (point.x * GRID_SIZE) - gridArea;
            int DrawY = (point.y * GRID_SIZE) - gridArea;

            if (!isSameZone(point.x, point.y - 1)) {
                DrawLineEx({(float)DrawX, (float)DrawY}, {(float)(DrawX + GRID_SIZE), (float)DrawY}, outlineThickness, outlineColor);
            }

            if (!isSameZone(point.x, point.y + 1)) {
                DrawLineEx({(float)DrawX, (float)(DrawY + GRID_SIZE)}, {(float)(DrawX + GRID_SIZE), (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
            }

            if (!isSameZone(point.x - 1, point.y)) {
                DrawLineEx({(float)DrawX, (float)DrawY}, {(float)DrawX, (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
            }
                    
            if (!isSameZone(point.x + 1, point.y)) {
                DrawLineEx({(float)(DrawX + GRID_SIZE), (float)DrawY}, {(float)(DrawX + GRID_SIZE), (float)(DrawY + GRID_SIZE)}, outlineThickness, outlineColor);
            }
        }
        int centerX = (int)((sumX / ownedCells.size()) * GRID_SIZE) - gridArea;
        int centerY = (int)((sumY / ownedCells.size()) * GRID_SIZE) - gridArea;

        DrawText("Not surrounded by walls!", centerX - 60, centerY + 20, 15, RED);
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