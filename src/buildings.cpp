#include "buildings.h"
#include "raymath.h"

void ZoneTemplate::draw(Camera2D camera)
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

    for (auto& point : ownedCells) 
    {
        if(point.x >=TL_drw.x && point.x <=BR_drw.x  && point.y >=TL_drw.y && point.y <=BR_drw.y )
        {
            int DrawX = (point.x * GRID_SIZE) - gridArea;
            int DrawY = (point.y * GRID_SIZE) - gridArea;
            if (who_am_I == 1)
            {
                DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, Fade(BLUE, 0.3f));
            }
            else
            {
                DrawRectangle(DrawX, DrawY, GRID_SIZE, GRID_SIZE, Fade(RED, 0.3f));
            }
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
    if (haveTexture)
    {
        DrawTexture(whatTexture, drawX,drawY, WHITE);    
    }
    return;
}