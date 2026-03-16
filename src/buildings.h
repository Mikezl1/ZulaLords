#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "constants.h"

struct point
{
    int x;
    int y;
};

struct ZoneTemplate
{
    std::string name;
    int who_am_I ; 
    int capacity;
    int price;
    Color color;
    void draw();

    std::vector<point> ownedCells;//jaký jsou bunky v zone
};