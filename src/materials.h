#pragma once
#include <vector>
#include <string> 
#include <raylib.h>
#include "constants.h"
#include "npc.h"

typedef enum {
    WOOD,
    PLANK,
    BRICK,
    STONE,
} Material_Type;

struct Materials {
    int amount;
    int type;
    int x, y;
    bool picked_up;
    int value;
    Color Barva;
    void Draw();
    // void Create();
};