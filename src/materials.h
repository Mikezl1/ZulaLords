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
    int type;
    int x, y;
    int id;
    bool picked_up;
    int value;
    Color Barva;
    void Draw();
    // void Create();
};