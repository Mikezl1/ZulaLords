#include "buildings.h"


void ZoneTemplate::draw()
{
    for (auto& point : ownedCells) {
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