#pragma once
#include <vector>
#include <string>
#include <raylib.h>
#include "constants.h"


Texture2D ExtractBlock(Image atlas, int poloz, int row)
{
    Rectangle cropRec = { (float)(poloz * 50), (float)(row * 50), 50.0f, 50.0f };
    Image piece = ImageFromImage(atlas, cropRec);
    Texture2D tex = LoadTextureFromImage(piece);
    UnloadImage(piece);
    return tex;
}

TexPack LoadTexPack(const char* fileName, Color color) 
{
    TexPack pack;
    pack.barv = color;

    Image chance = LoadImage(fileName);//200x300

    pack.center     = ExtractBlock(chance, 0, 0);
    pack.horizontal = ExtractBlock(chance, 1, 0);
    pack.vertically = ExtractBlock(chance, 2, 0);
    pack.full       = ExtractBlock(chance, 3, 0);

    pack.left       = ExtractBlock(chance, 0, 1);
    pack.right      = ExtractBlock(chance, 1, 1);
    pack.up         = ExtractBlock(chance, 2, 1);
    pack.down       = ExtractBlock(chance, 3, 1);

    pack.notup      = ExtractBlock(chance, 0, 2);
    pack.notdown    = ExtractBlock(chance, 1, 2);
    pack.notleft    = ExtractBlock(chance, 2, 2);
    pack.notright   = ExtractBlock(chance, 3, 2);

    pack.corner_leftup_E    = ExtractBlock(chance, 0, 3);
    pack.corner_rightup_E   = ExtractBlock(chance, 1, 3);
    pack.corner_leftdown_E  = ExtractBlock(chance, 2, 3);
    pack.corner_rightdown_E = ExtractBlock(chance, 3, 3);

    pack.corner_leftup_F    = ExtractBlock(chance, 0, 4);
    pack.corner_rightup_F   = ExtractBlock(chance, 1, 4);
    pack.corner_leftdown_F  = ExtractBlock(chance, 2, 4);
    pack.corner_rightdown_F = ExtractBlock(chance, 3, 4);

    pack.full_notleftup     = ExtractBlock(chance, 0, 5);
    pack.full_notrightup    = ExtractBlock(chance, 1, 5);
    pack.full_notleftdown   = ExtractBlock(chance, 2, 5);
    pack.full_notrightdown  = ExtractBlock(chance, 3, 5);

    UnloadImage(chance);

    return pack;
}