#include "buildings.h"


void BuildingTemplate::draw(int drawX,int drawY, bool rotate)
{
    if (rotate) {
        DrawRectangle(drawX - gridHeight/2 *GRID_SIZE, drawY - gridWidth/2 * GRID_SIZE, gridHeight * GRID_SIZE, gridWidth * GRID_SIZE, color);
    }
    else {
        DrawRectangle(drawX - gridWidth/2 * GRID_SIZE, drawY - gridHeight/2 *GRID_SIZE, gridWidth * GRID_SIZE, gridHeight * GRID_SIZE, color);
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