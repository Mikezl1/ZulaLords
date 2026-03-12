#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>

typedef enum {
    NPC_IDLE, NPC_WALKING, NPC_WALKING_TO_WORK, NPC_WORKING,
    NPC_WALKING_TO_HOME, NPC_SHOPPING, NPC_WALKING_TO_SHOP, NPC_HOME,
} NPC_doing;

typedef enum {
    FOREST, SAWMILL, FARM, FOOD_SHOP, GOODS_SHOP, NONE,
} NPC_work;

extern std::vector<Vector2> activeShops;

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
    NPC_work work;

    void NPC_movement(const std::vector<Vector2>& shops);
    void draw();
};