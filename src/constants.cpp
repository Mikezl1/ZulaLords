#include <vector>
#include <string>
#include <raylib.h>
#include "constants.h"


Rectangle GetQuadrantRect(int corner, int type) 
{
    float x = type * 50.0f;
    float y = 0.0f; 

    if (corner == 1 || corner == 3) x += 25.0f; 
    if (corner == 2 || corner == 3) y += 25.0f;

    return { x, y, 25.0f, 25.0f };
}

Texture2D ComposeTile(Image atlas, int tl, int tr, int bl, int br) {
    Image dst = GenImageColor(50, 50, BLANK);

    ImageDraw(&dst, atlas, GetQuadrantRect(0, tl), { 0, 0, 25, 25 }, WHITE);
    ImageDraw(&dst, atlas, GetQuadrantRect(1, tr), { 25, 0, 25, 25 }, WHITE);
    ImageDraw(&dst, atlas, GetQuadrantRect(2, bl), { 0, 25, 25, 25 }, WHITE);
    ImageDraw(&dst, atlas, GetQuadrantRect(3, br), { 25, 25, 25, 25 }, WHITE);

    Texture2D tex = LoadTextureFromImage(dst);
    UnloadImage(dst);
    return tex;
}

/*
 * ComposeTile(atlas, TL, TR, BL, BR)
    TL = Top lest  atd.....
 0 = Vnejsi roh 
 1 = Plny stred 
 2 = Horizontalni hrana 
 3 = Vertikalni hrana
 4 = Vnitrni roh 
 */

TexPack LoadTexPack(const char* fileName, Color col) 
{
    TexPack p;
    p.barv = col;
    
    //250x50 px
    Image atlas = LoadImage(fileName);
    ImageResizeNN(&atlas, 250, 50);
    
    p.center     = ComposeTile(atlas, 0, 0, 0, 0);
    p.horizontal = ComposeTile(atlas, 2, 2, 2, 2);
    p.vertically = ComposeTile(atlas, 3, 3, 3, 3);
    p.full       = ComposeTile(atlas, 1, 1, 1, 1);

    p.left  = ComposeTile(atlas, 0, 2, 0, 2);
    p.right = ComposeTile(atlas, 2, 0, 2, 0);
    p.up    = ComposeTile(atlas, 0, 0, 3, 3);
    p.down  = ComposeTile(atlas, 3, 3, 0, 0);

    p.notup    = ComposeTile(atlas, 2, 2, 1, 1);
    p.notdown  = ComposeTile(atlas, 1, 1, 2, 2);
    p.notleft  = ComposeTile(atlas, 3, 1, 3, 1);
    p.notright = ComposeTile(atlas, 1, 3, 1, 3);

    p.corner_leftup_E    = ComposeTile(atlas, 0, 2, 3, 4);
    p.corner_rightup_E   = ComposeTile(atlas, 2, 0, 4, 3);
    p.corner_leftdown_E  = ComposeTile(atlas, 3, 4, 0, 2);
    p.corner_rightdown_E = ComposeTile(atlas, 4, 3, 2, 0);

    p.corner_leftup_F    = ComposeTile(atlas, 0, 2, 3, 1);
    p.corner_rightup_F   = ComposeTile(atlas, 2, 0, 1, 3);
    p.corner_leftdown_F  = ComposeTile(atlas, 3, 1, 0, 2);
    p.corner_rightdown_F = ComposeTile(atlas, 1, 3, 2, 0);

    p.full_notleftup    = ComposeTile(atlas, 4, 1, 1, 1);
    p.full_notrightup   = ComposeTile(atlas, 1, 4, 1, 1);
    p.full_notleftdown  = ComposeTile(atlas, 1, 1, 4, 1);
    p.full_notrightdown = ComposeTile(atlas, 1, 1, 1, 4);

    p.full_not_top_corners    = ComposeTile(atlas, 4, 4, 1, 1);
    p.full_not_bottom_corners = ComposeTile(atlas, 1, 1, 4, 4);
    p.full_not_left_corners   = ComposeTile(atlas, 4, 1, 4, 1);
    p.full_not_right_corners  = ComposeTile(atlas, 1, 4, 1, 4);

    p.full_not_diag_1      = ComposeTile(atlas, 4, 1, 1, 4);
    p.full_not_diag_2      = ComposeTile(atlas, 1, 4, 4, 1);
    p.full_not_all_corners = ComposeTile(atlas, 4, 4, 4, 4);
    //p.completely_empty = LoadTextureFromImage(emptyImg);

    p.full_only_leftup    = ComposeTile(atlas, 1, 4, 4, 4);
    p.full_only_rightup   = ComposeTile(atlas, 4, 1, 4, 4);
    p.full_only_leftdown  = ComposeTile(atlas, 4, 4, 1, 4);
    p.full_only_rightdown = ComposeTile(atlas, 4, 4, 4, 1);

    p.notup_notleftdown  = ComposeTile(atlas, 2, 2, 4, 1);
    p.notup_notrightdown = ComposeTile(atlas, 2, 2, 1, 4);
    p.notdown_notleftup  = ComposeTile(atlas, 4, 1, 2, 2);
    p.notdown_notrightup = ComposeTile(atlas, 1, 4, 2, 2);

    p.notleft_notrightup   = ComposeTile(atlas, 3, 4, 3, 1);
    p.notleft_notrightdown = ComposeTile(atlas, 3, 1, 3, 4);
    p.notright_notleftup   = ComposeTile(atlas, 4, 3, 1, 3);
    p.notright_notleftdown = ComposeTile(atlas, 1, 3, 4, 3);

    p.notup_notboth    = ComposeTile(atlas, 2, 2, 4, 4);
    p.notdown_notboth  = ComposeTile(atlas, 4, 4, 2, 2);
    p.notleft_notboth  = ComposeTile(atlas, 3, 4, 3, 4);
    p.notright_notboth = ComposeTile(atlas, 4, 3, 4, 3);

    UnloadImage(atlas);
    return p;
}