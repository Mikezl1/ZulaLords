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
    NPC_BUILDING,
    NPC_GATHERING,
    NPC_LOGGING,
    NPC_MINING,
    NPC_FARMING,
} NPC_doing;

typedef enum {
    QUARRY,
    SAWMILL,
    FARM,
    NO_WORK,
} NPC_work;

class NPC {
public:
    char name[100];
    bool clicked = false;
    int x, y;
    NPC_doing doing;
    int rad;
    int age;
    int workplace;
    int workplaceX, workplaceY;
    int shopX, shopY;
    int homeX, homeY;
    int speedX, speedY;
    int startX, startY;
    int destinationX, destinationY;
    int itemID;
    int itemType;
    int work_zone_id;
    float waitTimer = 0.0f;
    float targetWaitTime = 0.0f;    
    bool hasahouse = false;
    bool registeredhouse = false;
    bool hasaworkplace = false;
    bool holdingitem = false;
    bool finished_work = false;
    bool found_source = false;
    NPC_work work;
    std::vector<Vector2> currentPath;
    long unsigned int currentPathIndex = 0;
    void NPC_movement(const std::vector<ZoneTemplate>& LiveZone, const std::vector<std::vector<Object>>& grid);
    void draw();
};