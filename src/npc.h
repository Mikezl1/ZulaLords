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
    NPC_HOME,
    NPC_BUILDING,
    NPC_GATHERING,
    NPC_LOGGING,
    NPC_MINING,
    NPC_FARMING,
    NPC_EATING,    // Colonist found food, moving to eat it
} NPC_doing;

typedef enum {
    FARM,
    STORAGE_HAUL,      // Haul materials to storage zone
    CONSTRUCTION,      // Build walls in construction zones
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
    int homeX, homeY;
    int speedX, speedY;
    int startX, startY;
    int destinationX, destinationY;
    int itemID;
    int itemType;
    int work_zone_id;
    float waitTimer = 0.0f;
    float targetWaitTime = 0.0f;    
    float hunger = 0.0f;   // 0 = full, 100 = starving
    int   food_id = -1;    // ID of food being pursued
    bool hasahouse = false;
    bool registeredhouse = false;
    bool hasaworkplace = false;
    bool holdingitem = false;
    bool finished_work = false;
    bool found_source = false;
    NPC_work work;
    int assigned_zone_id = -1;      // Zone this NPC is assigned to (-1 = none)
    int assigned_job_type = 0;      // Job type: 0=none, 1=farm, 2=storage_haul, etc.
    std::vector<Vector2> currentPath;
    long unsigned int currentPathIndex = 0;
    void NPC_movement(const std::vector<ZoneTemplate>& LiveZone, const std::vector<std::vector<Object>>& grid);
    void draw();
};