#pragma once
#include <vector>
#include <string> 
#include <raylib.h>
#include "constants.h"

typedef enum {
    CLEAR,
    HOUSE_ZONE,
    SHOP_ZONE,
    WORK_ZONE,
} ZoneType;


struct point
{
    int x;
    int y;
};

struct ZoneTemplate
{
    std::string name;
    int who_am_I;
    int capacity;
    int startX, startY, endX, endY;
    int zoneIndex;
    bool valid;
    Color color;
    ZoneType type;
    void CheckValidity(const std::vector<std::vector<Object>>& grid);
    void draw(Camera2D camera, const std::vector<std::vector<Object>>& grid);

    std::vector<point> ownedCells;//jaký jsou bunky v zone
};