#include "npc.h"
#include "buildings.h"
#include <cmath>


void NPC::draw()
{
    DrawCircle(x, y, rad, RED); 
    return;
}

void NPC::NPC_movement(const std::vector<Vector2>& shops, const std::vector<Vector2>& houseslocations) {
    if (hasahouse == false) {
        if (!houseslocations.empty()) {
            int closestDistance = -1;
            Vector2 closestHouse = {0, 0};
            Vector2 currentPosition = {(float)x, (float)y};

            for (size_t i = 0; i < houseslocations.size(); i++) {
                float dist = Vector2Distance(currentPosition, houseslocations[i]);

                if ((closestDistance < 0 || dist < closestDistance)) {
                    closestDistance = dist;
                    closestHouse = houseslocations[i];
                }
            }

            homeX = closestHouse.x;
            homeY = closestHouse.y;
            hasahouse = true;
            doing = NPC_WALKING_TO_HOME;
        }
    }
    if (doing == NPC_IDLE) {
        waitTimer += GetFrameTime();

        if (waitTimer >= targetWaitTime) {
            doing = NPC_WALKING;
            startX = x;
            startY = y;
            destinationX = GetRandomValue(-500, 500) + startX;
            destinationY = GetRandomValue(-500, 500) + startY;

            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_WORKING) {
        waitTimer += GetFrameTime();

        if (waitTimer >= targetWaitTime) {    
            doing = NPC_WALKING_TO_HOME;
            startX = x;
            startY = y;
            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_SHOPPING) {
        waitTimer += GetFrameTime();

        if (waitTimer >= targetWaitTime) {    
            doing = NPC_WALKING_TO_HOME;
            startX = x;
            startY = y;
            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_HOME) {
        waitTimer += GetFrameTime();

        if (waitTimer >= targetWaitTime) {    
            doing = NPC_WALKING_TO_SHOP;
            startX = x;
            startY = y;
            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_WALKING || doing == NPC_WALKING_TO_WORK || doing == NPC_WALKING_TO_HOME || doing == NPC_WALKING_TO_SHOP)
    {
        int speed = 3; 

        if (doing == NPC_WALKING_TO_WORK)
        {
            destinationX = workplaceX;
            destinationY = workplaceY;
        }
        else if (doing == NPC_WALKING_TO_HOME)
        {
            destinationX = homeX;
            destinationY = homeY;
        }
        else if (doing == NPC_WALKING_TO_SHOP)
        {
            float closestDistance = -1.0f;
            Vector2 closestShop = {0, 0};
            Vector2 currentPosition = {(float)x, (float)y};

            for (size_t i = 0; i < activeShops.size(); i++) {
                float dist = Vector2Distance(currentPosition, activeShops[i]);

                if (closestDistance < 0 || dist < closestDistance) {
                    closestDistance = dist;
                    closestShop = activeShops[i];
                }
            }


            destinationX = closestShop.x;
            destinationY = closestShop.y;
        }

        if (abs(destinationX - x) <= speed) {
            x = destinationX;
            speedX = 0;
        } 
        else if (x < destinationX) {
            x += speed;
            speedX = speed;
        } 
        else if (x > destinationX) {
            x -= speed;
            speedX = -speed;
        }

        if (abs(destinationY - y) <= speed) {
            y = destinationY;
            speedY = 0;
        } 
        else if (y < destinationY) {
            y += speed;
            speedY = speed;
        } 
        else if (y > destinationY) {
            y -= speed;
            speedY = -speed;
        }

        if (x == destinationX && y == destinationY) {
            if (doing == NPC_WALKING_TO_WORK) {
                doing = NPC_WORKING;
                targetWaitTime = 60;
            } 
            else if (doing == NPC_WALKING_TO_HOME) {
                doing = NPC_HOME;
                targetWaitTime = 20;
            }
            else if (doing == NPC_WALKING_TO_SHOP) {
                doing = NPC_SHOPPING;
                targetWaitTime = 30;
            }
            else {
                doing = NPC_IDLE; 
                targetWaitTime = (float)GetRandomValue(1, 8);
            }
        }
    }    
}