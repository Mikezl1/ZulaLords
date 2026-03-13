#pragma once

#define GRID_SIZE 50 // Velikost ctverecku
#define gridArea  50000// Velikost gridu
#define cells gridArea * 2 / GRID_SIZE// počet buněk nebo kostek

typedef enum {
    TERRAIN_BLANK,
    TERRAIN_DIRT_PATH,
    TERRAIN_STONE_PATH,
    TERRAIN_LIMESTONE_WALL,
    TERRAIN_COUNT
} TerrainType;

const Color TerrainColors[] = {
    [TERRAIN_BLANK] = {0, 0, 0, 0},
    [TERRAIN_DIRT_PATH] = {139, 69, 19, 255},
    [TERRAIN_STONE_PATH]  = {136,140,141, 255}, 
    [TERRAIN_LIMESTONE_WALL] = {217, 211, 199, 255}
};

inline bool operator!=(const Color& a, const Color& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

class Object {
public:
    Color barv;
    int x, y;
    int drawX, drawY;
    void draw();
};