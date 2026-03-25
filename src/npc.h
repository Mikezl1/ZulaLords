#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include "constants.h"
#include "materials.h"
#include "buildings.h"

typedef enum {
    NPC_IDLE,
    NPC_WALKING,
    NPC_WALKING_TO_WORK,
    NPC_WORKING,
    NPC_WALKING_TO_HOME,
    NPC_SHOPPING,
    NPC_WALKING_TO_SHOP,
    NPC_HOME,
} NPC_doing;

typedef enum {
    FOREST,
    SAWMILL,
    FARM,
    SHOP,
    NONE,
} NPC_work;

class NPC {
public:
    char name[100];
    bool clicked = false;
    int x, y;
    NPC_doing doing;
    int amount;
    int rad;
    int age;
    int workplace;
    int workplaceX, workplaceY;
    int shopX, shopY;
    int homeX, homeY;
    int speedX, speedY;
    int startX, startY;
    int destinationX, destinationY;
    float waitTimer = 0.0f;
    float targetWaitTime = 0.0f;    
    bool hasahouse = false;
    bool hasaworkplace = false;
    NPC_work work;
    std::vector<Vector2> currentPath;
    long unsigned int currentPathIndex = 0;
    void NPC_movement(const std::vector<ZoneTemplate>& LiveZone, const std::vector<std::vector<Object>>& grid);
    void draw();
};