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

inline bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

class TexPack{
public:
    Color barv;
    Texture2D center;
    Texture2D left;
    Texture2D right;
    Texture2D horizontal;
    Texture2D up;
    Texture2D down;
    Texture2D vertically;
    Texture2D full;
    Texture2D notup;
    Texture2D notdown;
    Texture2D notleft;
    Texture2D notright;

    Texture2D corner_leftup_E;//E = empty
    Texture2D corner_rightup_E;
    Texture2D corner_leftdown_E;
    Texture2D corner_rightdown_E;

    Texture2D corner_leftup_F;//F full
    Texture2D corner_rightup_F;
    Texture2D corner_leftdown_F;
    Texture2D corner_rightdown_F;

};

class Object {
public:
    Color barv;
    bool haveTexture;
    TexPack textura;
    int x, y;
    int drawX, drawY;
    void draw();
    void drawTextures(Texture2D whatTexture);
};