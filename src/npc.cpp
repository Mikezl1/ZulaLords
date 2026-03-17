#include "npc.h"
#include "buildings.h"
#include <cmath>


void NPC::draw()
{
    DrawCircle(x, y, rad, RED); 
    return;
}

void NPC::NPC_movement(const std::vector<ZoneTemplate>& LiveZone, const std::vector<std::vector<Object>>& grid) 
{
    // --- 1. EVICTION CHECK ---
    if (hasahouse == true) {
        int gridX = (homeX + gridArea) / GRID_SIZE;
        int gridY = (homeY + gridArea) / GRID_SIZE;
        bool houseStillThere = false;
        
        if (gridX >= 0 && gridX < cells && gridY >= 0 && gridY < cells) {
            // Check if the center coordinate is still within a House Zone
            if (grid[gridX][gridY].am_I_zone == true && grid[gridX][gridY].what_am_I == HOUSE_ZONE) {
                houseStillThere = true;
            }
        }

        if (!houseStillThere) {
            hasahouse = false;
            if (doing == NPC_WALKING_TO_HOME || doing == NPC_HOME) {
                doing = NPC_IDLE; 
                waitTimer = 0.0f;
            }
        }
    }

    // --- 2. HOUSE HUNTING (FINDING THE CENTER) ---
    if (hasahouse == false) {
        float closestDistance = -1;
        Vector2 bestCenter = {0, 0};
        bool foundAnyHouse = false;
        Vector2 currentPosition = {(float)x, (float)y};

        // Loop through all zones instead of individual tiles
        for (const auto& zone : LiveZone) {
            // Only look at House zones that actually have cells
            if (zone.type == HOUSE_ZONE && !zone.ownedCells.empty()) {
                
                // Calculate the mathematical center of this specific zone
                float sumX = 0;
                float sumY = 0;
                for (const auto& p : zone.ownedCells) {
                    sumX += p.x;
                    sumY += p.y;
                }
                float centerGridX = sumX / zone.ownedCells.size();
                float centerGridY = sumY / zone.ownedCells.size();

                // Convert the grid center back to world drawing coordinates
                Vector2 zoneCenter;
                zoneCenter.x = (centerGridX * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                zoneCenter.y = (centerGridY * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);

                // Check if this center is the closest one we've found
                float dist = Vector2Distance(currentPosition, zoneCenter);
                if (closestDistance < 0 || dist < closestDistance) {
                    closestDistance = dist;
                    bestCenter = zoneCenter;
                    foundAnyHouse = true;
                }
            }
        }

        if (foundAnyHouse) {
            homeX = bestCenter.x;
            homeY = bestCenter.y;
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
            doing = NPC_WALKING_TO_HOME;
            startX = x;
            startY = y;
            waitTimer = 0.0f;
        }
        else {
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
        // else if (doing == NPC_WALKING_TO_SHOP)
        // {
        //     float closestDistance = -1.0f;
        //     Vector2 closestShop = {0, 0};
        //     Vector2 currentPosition = {(float)x, (float)y};

        //     for (size_t i = 0; i < activeShops.size(); i++) {
        //         float dist = Vector2Distance(currentPosition, activeShops[i]);

        //         if (closestDistance < 0 || dist < closestDistance) {
        //             closestDistance = dist;
        //             closestShop = activeShops[i];
        //         }
        //     }

        //     destinationX = closestShop.x;
        //     destinationY = closestShop.y;
        // }

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