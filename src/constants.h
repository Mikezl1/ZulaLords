#pragma once

#define GRID_SIZE 50 // Velikost ctverecku
#define gridArea  50000// Velikost gridu
#define cells gridArea * 2 / GRID_SIZE// počet buněk nebo kostek


const int PRICE_STONE_WALL = 5;
const int PRICE_WOODEN_WALL = 2;

typedef enum {
    TERRAIN_BLANK,
    TERRAIN_DIRT_PATH,
    TERRAIN_STONE_PATH,
    TERRAIN_LIMESTONE_WALL,
    TERRAIN_COUNT,
} TerrainType;

const Color TerrainColors[] = {
    [TERRAIN_BLANK] = {0, 0, 0, 0},
    [TERRAIN_DIRT_PATH] = {139, 69, 19, 255},
    [TERRAIN_STONE_PATH]  = {136,140,141, 255}, 
    [TERRAIN_LIMESTONE_WALL] = {217, 211, 199, 255}
};

typedef enum {
    Wall_BLANK,
    Wall_Stone = 101,
    Wall_Wooden,
    Wall_COUNT,
} WallType;


inline bool operator!=(const Color& a, const Color& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

inline bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

class TexPack{//24
public:
    Color barv;
    Texture2D center; Texture2D horizontal; Texture2D vertically; Texture2D full;
    Texture2D left; Texture2D right; Texture2D up; Texture2D down;
    Texture2D notup; Texture2D notdown; Texture2D notleft; Texture2D notright;
    Texture2D corner_leftup_E; Texture2D corner_rightup_E; Texture2D corner_leftdown_E; Texture2D corner_rightdown_E;
    Texture2D corner_leftup_F; Texture2D corner_rightup_F; Texture2D corner_leftdown_F; Texture2D corner_rightdown_F;
    Texture2D full_notleftup; Texture2D full_notrightup; Texture2D full_notleftdown; Texture2D full_notrightdown;
    Texture2D full_not_top_corners; Texture2D full_not_bottom_corners; Texture2D full_not_left_corners; Texture2D full_not_right_corners;
    Texture2D full_not_diag_1; Texture2D full_not_diag_2; Texture2D full_not_all_corners; Texture2D completely_empty;
    Texture2D full_only_leftup; Texture2D full_only_rightup; Texture2D full_only_leftdown; Texture2D full_only_rightdown;
    Texture2D notup_notleftdown; Texture2D notup_notrightdown; Texture2D notdown_notleftup; Texture2D notdown_notrightup;
    Texture2D notleft_notrightup; Texture2D notleft_notrightdown; Texture2D notright_notleftup; Texture2D notright_notleftdown;
    Texture2D notup_notboth; Texture2D notdown_notboth; Texture2D notleft_notboth; Texture2D notright_notboth;

};

class Object {
public:
    Color barv;
    int what_am_I;
    bool am_I_zone;///jsem zona?????
    int myzone;// index of what zone it belongs to
    bool haveTexture;
    TexPack textura;
    bool built = false;
    int x, y;
    int drawX, drawY;
    void draw();
    void drawTextures(Texture2D whatTexture);
};


TexPack LoadTexPack(const char* fileName, Color color) ;