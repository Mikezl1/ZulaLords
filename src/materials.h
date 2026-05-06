#pragma once
#include <vector>
#include <string> 
#include <raylib.h>
#include "constants.h"
#include "npc.h"

typedef enum {
    WOOD,
    BRICK,
    STONE,
    FOOD,   // Harvested crops — colonists eat to restore hunger
} Material_Type;

struct Materials {
    int type;
    float x, y;
    int id;
    bool picked_up;
    bool walking_towards;
    int value;
    Color Barva;
    void Draw();
    // void Create();
};