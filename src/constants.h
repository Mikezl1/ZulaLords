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

class TexPack{//24
public:
    Color barv;

    Texture2D center;//1
    Texture2D horizontal;//2   
    Texture2D vertically;  //3  
    Texture2D full;//4

    Texture2D left;
    Texture2D right;
    Texture2D up;
    Texture2D down;

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

    Texture2D full_notleftup;// full but not
    Texture2D full_notrightup;
    Texture2D full_notleftdown;
    Texture2D full_notrightdown;
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

Texture2D ExtractBlock(Image atlas, int poloz, int row);

TexPack LoadTexPack(const char* fileName, Color color) ;