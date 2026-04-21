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
    bool anyvalidzone;

    for (const auto& zone : LiveZone) {
        if (zone.valid && !zone.ownedCells.empty()) {
            anyvalidzone = true;
        }
    }
    if (hasahouse == true && !(doing == NPC_WALKING_TO_SHOP || doing == NPC_WALKING_TO_WORK || doing == NPC_SHOPPING || doing == NPC_WORKING || doing == NPC_LOGGING || doing == NPC_MINING || doing == NPC_FARMING) && anyvalidzone) {
        int gridX = (homeX + gridArea) / GRID_SIZE;
        int gridY = (homeY + gridArea) / GRID_SIZE;
        bool houseStillThere = false;
        
        if (x == homeX && y == homeY) {
            if (grid[gridX][gridY].am_I_zone && grid[gridX][gridY].what_am_I == HOUSE_ZONE) {
                houseStillThere = true;
            }
        }

        if (houseStillThere == false) {
            hasahouse = false;
            homeX = 0;
            homeY = 0;
            if (doing == NPC_WALKING_TO_HOME || doing == NPC_HOME) {
                doing = NPC_IDLE;
                waitTimer = 0.0f;
            }
        }
    }

    if (hasahouse == false) {
        float closestDistance = -1;
        Vector2 bestCenter = {0, 0};
        bool foundAnyHouse = false;
        Vector2 currentPosition = {(float)x, (float)y};

        for (const auto& zone : LiveZone) {
            if (zone.type == HOUSE_ZONE && !zone.ownedCells.empty() && zone.valid == true && (zone.capacity > 0 || registeredhouse)) {
                
                float sumX = 0;
                float sumY = 0;
                for (const auto& p : zone.ownedCells) {
                    sumX += p.x;
                    sumY += p.y;
                }
                float centerGridX = sumX / zone.ownedCells.size();
                float centerGridY = sumY / zone.ownedCells.size();

                Vector2 zoneCenter;
                zoneCenter.x = (centerGridX * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                zoneCenter.y = (centerGridY * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);

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

        if (finished_work) {    
            doing = NPC_WALKING_TO_HOME;
            startX = x;
            startY = y;
            waitTimer = 0.0f;
            finished_work = false;
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
            bool shopExists = false;
            for (const auto& zone : LiveZone) {
                if (zone.type == SHOP_ZONE && !zone.ownedCells.empty() && zone.valid) {
                    shopExists = true;
                    break;
                }
            }

            if (!shopExists) {
                // If there are no shops, just go idle and wander around
                doing = NPC_IDLE;
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

            bool workExists = false;

            for (const auto& zone : LiveZone) {
                if ((zone.type == WORK_SAWMILL_ZONE || zone.type == WORK_QUARRY_ZONE) && !zone.ownedCells.empty() && zone.valid) {
                    workExists = true;
                    break;
                }
            }

            if (workExists || hasaworkplace) {
                startX = x;
                startY = y;
                doing = NPC_WALKING_TO_WORK;
                waitTimer = 0.0f;
            }
        }
    }
    else if (doing != NPC_HOME || doing != NPC_SHOPPING)
    {
        int speed = 3;

        if (doing == NPC_WALKING_TO_WORK)
        {
            if (hasaworkplace) {
                destinationX = workplaceX;
                destinationY = workplaceY;
            }
            else {
                float closestDistance = -1.0f;
                Vector2 bestWorkCenter = {0, 0};
                bool foundanywork = false;
                Vector2 currentPosition = {(float)x, (float)y};

                for (const auto& zone : LiveZone) {
                    if ((zone.type == WORK_SAWMILL_ZONE || zone.type == WORK_QUARRY_ZONE) && !zone.ownedCells.empty() && zone.valid) {
                        
                        float sumX = 0, sumY = 0;
                        for (const auto& p : zone.ownedCells) {
                            sumX += p.x;
                            sumY += p.y;
                        }
                        float centerGridX = sumX / zone.ownedCells.size();
                        float centerGridY = sumY / zone.ownedCells.size();

                        Vector2 zoneCenter;
                        zoneCenter.x = (centerGridX * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                        zoneCenter.y = (centerGridY * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);

                        float dist = Vector2Distance(currentPosition, zoneCenter);
                        if (closestDistance < 0 || dist < closestDistance) {
                            closestDistance = dist;
                            bestWorkCenter = zoneCenter;
                            foundanywork = true;
                            work_zone_id = zone.zoneIndex;

                        }
                    }
                }

                if (foundanywork) {
                    destinationX = bestWorkCenter.x;
                    destinationY = bestWorkCenter.y;
                    workplaceX = bestWorkCenter.x;
                    workplaceY = bestWorkCenter.y;
                    hasaworkplace = true;
                    for (const auto& zone : LiveZone) {
                        if (work_zone_id == zone.zoneIndex && zone.type == WORK_SAWMILL_ZONE) {
                            work = SAWMILL;
                            TraceLog(LOG_INFO, "Found work place: Sawnmill at X: %d Y: %d", workplaceX, workplaceY);
                        }
                        else if (work_zone_id == zone.zoneIndex && zone.type == WORK_QUARRY_ZONE) {
                            work = QUARRY;
                            TraceLog(LOG_INFO, "Found work place: Quarry at X: %d Y: %d", workplaceX, workplaceY);
                        }
                    }

                } else {
                    doing = NPC_IDLE; 
                }
            }
        }
        else if (doing == NPC_WALKING_TO_HOME)
        {
            destinationX = homeX;
            destinationY = homeY;
        }
        else if (doing == NPC_WALKING_TO_SHOP)
        {
            float closestDistance = -1.0f;
            Vector2 bestShopCenter = {0, 0};
            bool foundAnyShop = false;
            Vector2 currentPosition = {(float)x, (float)y};

            // Search the LiveZone array for the closest SHOP_ZONE center
            for (const auto& zone : LiveZone) {
                if (zone.type == SHOP_ZONE && !zone.ownedCells.empty() && zone.valid) {
                    
                    // Calculate the mathematical center of the shop zone
                    float sumX = 0, sumY = 0;
                    for (const auto& p : zone.ownedCells) {
                        sumX += p.x;
                        sumY += p.y;
                    }
                    float centerGridX = sumX / zone.ownedCells.size();
                    float centerGridY = sumY / zone.ownedCells.size();

                    Vector2 zoneCenter;
                    zoneCenter.x = (centerGridX * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                    zoneCenter.y = (centerGridY * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);

                    // Is this shop closer than the last one we checked?
                    float dist = Vector2Distance(currentPosition, zoneCenter);
                    if (closestDistance < 0 || dist < closestDistance) {
                        closestDistance = dist;
                        bestShopCenter = zoneCenter;
                        foundAnyShop = true;
                    }
                }
            }

            if (foundAnyShop) {
                destinationX = bestShopCenter.x;
                destinationY = bestShopCenter.y;
            } else {
                doing = NPC_IDLE; 
            }
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
                //targetWaitTime = 30;
            } 
            else if (doing == NPC_WALKING_TO_HOME) {
                doing = NPC_HOME;
                targetWaitTime = 10;
            }
            else if (doing == NPC_WALKING_TO_SHOP) {
                doing = NPC_SHOPPING;
                targetWaitTime = 10;
            }
            else {
                doing = NPC_IDLE; 
                targetWaitTime = (float)GetRandomValue(1, 8);
            }
        }
    }    
}