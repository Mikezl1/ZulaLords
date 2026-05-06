#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "constants.h"

typedef enum {
    CLEAR,
    HOUSE_ZONE,
    WORK_FARM_ZONE,   // Farming — colonists plant/harvest crops here
    STORAGE_ZONE,     // Central stockpile — all deliveries go here
    SHOP_ZONE,
} ZoneType;

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