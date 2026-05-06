#include "npc.h"
#include "buildings.h"
#include <cmath>


void NPC::draw()
{
    Color bodyColor;
    switch (doing) {
        case NPC_IDLE:             bodyColor = {100, 200, 100, 255}; break;
        case NPC_WALKING:          bodyColor = {150, 150, 200, 255}; break;
        case NPC_WALKING_TO_HOME:
        case NPC_HOME:             bodyColor = {200, 200, 100, 255}; break;
        case NPC_WALKING_TO_WORK:
        case NPC_WORKING:          bodyColor = {255, 180, 50,  255}; break;
        case NPC_LOGGING:          bodyColor = {139, 90,  43,  255}; break;
        case NPC_MINING:           bodyColor = {136, 140, 141, 255}; break;
        case NPC_FARMING:          bodyColor = {34,  139, 34,  255}; break; // green (farming)
        case NPC_GATHERING:
        case NPC_BUILDING:         bodyColor = {220, 120, 50,  255}; break;
        case NPC_EATING:           bodyColor = {255, 220, 80,  255}; break; // bright yellow (eating)
        default:                   bodyColor = RED;                   break;
    }

    // Body and outline
    DrawCircle(x, y, rad, bodyColor);
    DrawCircleLines(x, y, rad, {0, 0, 0, 120});

    // Held item indicator
    if (holdingitem) {
        Color itemCol = (itemType == WOOD) ? Color{139, 90, 43, 255} :
                            (itemType == STONE) ? Color{136, 140, 141, 255} :
                            Color{80, 200, 80, 255};
        DrawRectangle(x - 6, y - rad - 14, 12, 12, itemCol);
        DrawRectangleLines(x - 6, y - rad - 14, 12, 12, WHITE);
    }
    
    // Assigned job indicator (small icon above head)
    if (assigned_zone_id >= 0 && assigned_job_type > 0) {
        int iconY = y - rad - 18;
        Color jobColor = {255, 255, 0, 200}; // Yellow for assigned job
        
        // Draw small job indicator
        if (assigned_job_type == 1) { // FARM
            DrawCircle(x, iconY, 4, jobColor);
        } else if (assigned_job_type == 2) { // STORAGE_HAUL
            DrawRectangle(x - 3, iconY - 4, 6, 8, jobColor);
        }
    }
    
    return;
}

void NPC::NPC_movement(const std::vector<ZoneTemplate>& LiveZone, const std::vector<std::vector<Object>>& grid) 
{
    bool anyvalidzone = false;

    for (const auto& zone : LiveZone) {
        if (zone.valid && !zone.ownedCells.empty()) {
            anyvalidzone = true;
        }
    }
    if (hasahouse == true && !(doing == NPC_WALKING_TO_WORK || doing == NPC_WORKING || doing == NPC_LOGGING || doing == NPC_MINING || doing == NPC_FARMING) && anyvalidzone) {
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

    // Check for assigned zone first - prioritize assigned jobs
    if (assigned_zone_id >= 0) {
        // Find the assigned zone
        for (const auto& zone : LiveZone) {
            if (zone.zoneIndex == assigned_zone_id && zone.valid && !zone.ownedCells.empty()) {
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

                float dist = Vector2Distance(Vector2{(float)x, (float)y}, zoneCenter);
                
                // If not at assigned zone and not currently working, walk there
                if (doing == NPC_IDLE || doing == NPC_WALKING) {
                    doing = NPC_WALKING_TO_WORK;
                    workplaceX = zoneCenter.x;
                    workplaceY = zoneCenter.y;
                    workplace = zone.type;
                    hasaworkplace = true;
                }
                
                // If at assigned zone, do the assigned job
                if (doing == NPC_WORKING && dist < GRID_SIZE) {
                    TraceLog(LOG_INFO, "NPC %s performing assigned job in zone %d", name, assigned_zone_id);
                }
                
                break;
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
    else if (doing == NPC_HOME) {
        waitTimer += GetFrameTime();

        if (waitTimer >= targetWaitTime) {
            bool workExists = false;
            int bestWorkZoneIndex = -1;
            Vector2 bestWorkCenter = {0, 0};
            float bestDist = -1.0f;
            Vector2 currentPosition = {(float)x, (float)y};

            for (const auto& zone : LiveZone) {
                if ((zone.type == WORK_FARM_ZONE) && !zone.ownedCells.empty() && zone.valid) {
                    workExists = true;
                    
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
                    if (bestDist < 0 || dist < bestDist) {
                        bestDist = dist;
                        bestWorkCenter = zoneCenter;
                        bestWorkZoneIndex = zone.zoneIndex;
                    }
                }
            }

            if (workExists || hasaworkplace) {
                startX = x;
                startY = y;
                doing = NPC_WALKING_TO_WORK;
                waitTimer = 0.0f;
                
                // Assign work when arriving at farm zone
                workplaceX = bestWorkCenter.x;
                workplaceY = bestWorkCenter.y;
                workplace = WORK_FARM_ZONE;
            } else {
                doing = NPC_IDLE;
                waitTimer = 0.0f;
            }
        }
    }
    else if (doing != NPC_HOME)
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
                    if ((zone.type == WORK_FARM_ZONE) && !zone.ownedCells.empty() && zone.valid) {
                        
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
                        if (work_zone_id == zone.zoneIndex && zone.type == WORK_FARM_ZONE) {
                            work = FARM;
                            TraceLog(LOG_INFO, "Found work place: Farm at X: %d Y: %d", workplaceX, workplaceY);
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
            } 
            else if (doing == NPC_WALKING_TO_HOME) {
                doing = NPC_HOME;
                targetWaitTime = 10;
            }
            else if (doing == NPC_LOGGING || doing == NPC_MINING || doing == NPC_FARMING ||
                     doing == NPC_BUILDING || doing == NPC_GATHERING || doing == NPC_EATING) {
                // main.cpp handles transitions for these states via distance checks
                // Do NOT auto-transition to IDLE
            }
            else {
                doing = NPC_IDLE; 
                targetWaitTime = (float)GetRandomValue(1, 8);
            }
        }
    }    
}