#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "constants.h"

struct BuildingTemplate
{
    std::string name;
    int gridWidth;
    int gridHeight;
    int capacity;
    int price;
    Color color;
    Texture2D textura;
    void draw(int drawX, int drawY, bool rotate);
    void SetPosition(int x, int y)
    {
        drawX = x; drawY = y;
        return;
    }
    int drawX , drawY;
    void drawTexture()
    {
        DrawTexture(textura, drawX,drawY, WHITE);
    }
};