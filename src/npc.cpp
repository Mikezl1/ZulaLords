#include "npc.h"
#include "buildings.h"
#include "constants.h"
#include <cmath>
#include <queue>
#include <algorithm>





struct Point2D {
    int x, y;
    bool operator==(const Point2D& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Point2D& o) const { return !(*this == o); }
};

struct PathNode {
    Point2D pos;
    int g, h, f;
    bool operator>(const PathNode& other) const { return f > other.f; }
};

int getTerrainCost(Color c) {
    // Check if it's a path (Cheap to walk on)
    if (c.r == TerrainColors[TERRAIN_DIRT_PATH].r && c.g == TerrainColors[TERRAIN_DIRT_PATH].g) return 1;
    if (c.r == TerrainColors[TERRAIN_STONE_PATH].r && c.g == TerrainColors[TERRAIN_STONE_PATH].g) return 1;
    
    // Check if it's blank grass (Normal cost)
    if (c.a == TerrainColors[TERRAIN_BLANK].a) return 5; 
    
    // If it's anything else (Buildings, Walls), it is impassable
    return -1; 
}

int getHeuristic(Point2D a, Point2D b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

std::vector<Vector2> NPC::findPath(Vector2 startPos, Vector2 targetPos, const std::vector<std::vector<Object>>& grid) {
    // 1. Convert World Coordinates to Grid Coordinates
    int startX = ((int)startPos.x + gridArea) / GRID_SIZE;
    int startY = ((int)startPos.y + gridArea) / GRID_SIZE;
    int targetX = ((int)targetPos.x + gridArea) / GRID_SIZE;
    int targetY = ((int)targetPos.y + gridArea) / GRID_SIZE;

    // Clamp to bounds to prevent crashes
    if (startX < 0) startX = 0; 
    if (startX >= cells) startX = cells - 1;
    if (startY < 0) startY = 0; 
    if (startY >= cells) startY = cells - 1;
    if (targetX < 0) targetX = 0; 
    if (targetX >= cells) targetX = cells - 1;
    if (targetY < 0) targetY = 0; 
    if (targetY >= cells) targetY = cells - 1;

    Point2D start = {startX, startY};
    Point2D target = {targetX, targetY};

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openList;
    std::vector<std::vector<bool>> closedList(cells, std::vector<bool>(cells, false));
    std::vector<std::vector<Point2D>> parents(cells, std::vector<Point2D>(cells, {-1, -1}));

    openList.push({start, 0, getHeuristic(start, target), getHeuristic(start, target)});

    Point2D directions[4] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (!openList.empty()) {
        PathNode current = openList.top();
        openList.pop();

        // 2. Target Reached! Reconstruct the path.
        if (current.pos == target) {
            std::vector<Vector2> path;
            Point2D curr = target;
            while (curr != start) {
                // Convert back to World Coordinates (centered on the tile)
                float worldX = (curr.x * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                float worldY = (curr.y * GRID_SIZE) - gridArea + (GRID_SIZE / 2.0f);
                path.push_back({worldX, worldY});
                curr = parents[curr.x][curr.y];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (closedList[current.pos.x][current.pos.y]) continue;
        closedList[current.pos.x][current.pos.y] = true;

        // 3. Check Neighbors
        for (auto& dir : directions) {
            int nx = current.pos.x + dir.x;
            int ny = current.pos.y + dir.y;

            if (nx >= 0 && nx < cells && ny >= 0 && ny < cells && !closedList[nx][ny]) {
                int cost = getTerrainCost(grid[nx][ny].barv);
                
                if (cost >= 9999) continue; // Skip impassable terrain (buildings/walls)

                int newG = current.g + cost;
                int newH = getHeuristic({nx, ny}, target);
                parents[nx][ny] = current.pos;
                openList.push({{nx, ny}, newG, newH, newG + newH});
            }
        }
    }
    return {}; // Return empty if no path is found
}

void NPC::draw()
{
    if (!currentPath.empty() && currentPathIndex < currentPath.size()) {
        
        // Draw a line from the NPC's current physical position to its immediate next waypoint
        DrawLineEx({(float)x, (float)y}, currentPath[currentPathIndex], 2.0f, Fade(BLUE, 0.5f));

        // Draw lines connecting the rest of the waypoints in the path
        for (size_t i = currentPathIndex; i < currentPath.size() - 1; i++) {
            DrawLineEx(currentPath[i], currentPath[i+1], 2.0f, Fade(BLUE, 0.5f));
            
            // Optional: Draw a tiny dot at each grid cell intersection
            DrawCircleV(currentPath[i], 2.0f, Fade(RED, 0.5f)); 
        }

        // Draw a green marker at the final destination
        DrawCircleV(currentPath.back(), 4.0f, GREEN);
    }

    DrawCircle(x, y, rad, RED); 
    return;
}

void NPC::NPC_movement(const std::vector<Vector2>& shops, const std::vector<Vector2>& houseslocations, const std::vector<std::vector<Object>>& grid) {
    if (hasahouse == false) {
        if (!houseslocations.empty()) {
            int closestDistance = -1;
            Vector2 closestHouse = {0, 0};
            Vector2 currentPosition = {(float)x, (float)y};

            for (size_t i = 0; i < houseslocations.size(); i++) {
                float dist = Vector2Distance(currentPosition, houseslocations[i]);
                if (closestDistance < 0 || dist < closestDistance) {
                    closestDistance = dist;
                    closestHouse = houseslocations[i];
                }
            }
            homeX = closestHouse.x;
            homeY = closestHouse.y;
            hasahouse = true;
            doing = NPC_WALKING_TO_HOME;
            
            // Generate Path!
            currentPath = findPath({(float)x, (float)y}, closestHouse, grid);
            currentPathIndex = 0;
        }
    }


    if (doing == NPC_IDLE) {
        waitTimer += GetFrameTime();
        if (waitTimer >= targetWaitTime) {
            doing = NPC_WALKING;
            Vector2 randomDest = {(float)(GetRandomValue(-500, 500) + x), (float)(GetRandomValue(-500, 500) + y)};
            currentPath = findPath({(float)x, (float)y}, randomDest, grid);
            currentPathIndex = 0;
            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_WORKING || doing == NPC_SHOPPING || doing == NPC_HOME) {
        waitTimer += GetFrameTime();
        if (waitTimer >= targetWaitTime) {    
            Vector2 nextDest;
            if (doing == NPC_HOME) {
                doing = NPC_WALKING_TO_SHOP;
                
                // Find closest shop
                float closestDistance = -1.0f;
                Vector2 closestShop = {0, 0};
                for (size_t i = 0; i < shops.size(); i++) {
                    float dist = Vector2Distance({(float)x, (float)y}, shops[i]);
                    if (closestDistance < 0 || dist < closestDistance) {
                        closestDistance = dist;
                        closestShop = shops[i];
                    }
                }
                nextDest = closestShop;
            } else {
                doing = NPC_WALKING_TO_HOME;
                nextDest = {(float)homeX, (float)homeY};
            }
            
            currentPath = findPath({(float)x, (float)y}, nextDest, grid);
            currentPathIndex = 0;
            waitTimer = 0.0f;
        }
    }
    else if (doing == NPC_WALKING || doing == NPC_WALKING_TO_WORK || doing == NPC_WALKING_TO_HOME || doing == NPC_WALKING_TO_SHOP) {
        
        // If we still have waypoints left in our path
        if (!currentPath.empty() && currentPathIndex < currentPath.size()) {
            int destX = currentPath[currentPathIndex].x;
            int destY = currentPath[currentPathIndex].y;
            
            // --- THE SPEED BOOST CHECK ---
            int gridX = (x + gridArea) / GRID_SIZE;
            int gridY = (y + gridArea) / GRID_SIZE;
            int currentSpeed = 3; 

            if (gridX >= 0 && gridX < cells && gridY >= 0 && gridY < cells) {
                Color c = grid[gridX][gridY].barv;
                if ((c.r == TerrainColors[TERRAIN_DIRT_PATH].r && c.g == TerrainColors[TERRAIN_DIRT_PATH].g) ||
                    (c.r == TerrainColors[TERRAIN_STONE_PATH].r && c.g == TerrainColors[TERRAIN_STONE_PATH].g)) {
                    currentSpeed = 6; // Boost speed on paths!
                }
            }

            // Move X
            if (abs(destX - x) <= currentSpeed) {
                x = destX;
            } else if (x < destX) {
                x += currentSpeed;
            } else if (x > destX) {
                x -= currentSpeed;
            }

            // Move Y
            if (abs(destY - y) <= currentSpeed) {
                y = destY;
            } else if (y < destY) {
                y += currentSpeed;
            } else if (y > destY) {
                y -= currentSpeed;
            }

            // If reached the current waypoint, target the next one
            if (x == destX && y == destY) {
                currentPathIndex++;
            }
        } 
        // If we reached the final destination in the path
        else {
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
            currentPath.clear(); // Clear the path once arrived
        }
    }    
}