#include "materials.h"

void Materials::Draw() {
    Color col;
    switch (type) {
        case WOOD:  col = {139, 90,  43,  255}; break; // brown log
        case STONE: col = {136, 140, 141, 255}; break; // grey stone
        case FOOD:  col = {80,  200, 80,  255}; break; // green crop
        default:    col = Barva;                 break;
    }
    if (picked_up) return; // Don't draw carried items (NPC draw shows them)

    // Draw resource pile with outline
    DrawRectangle((int)x - 6, (int)y - 6, 12, 12, col);
    DrawRectangleLines((int)x - 6, (int)y - 6, 12, 12, {0, 0, 0, 100});

    // Type indicator
    if (type == FOOD) {
        // Small leaf detail on food piles
        DrawCircle((int)x, (int)y - 2, 3, {50, 160, 50, 200});
    } else if (type == WOOD) {
        // Horizontal grain lines
        DrawLine((int)x - 5, (int)y - 2, (int)x + 5, (int)y - 2, {100, 60, 20, 150});
        DrawLine((int)x - 5, (int)y + 2, (int)x + 5, (int)y + 2, {100, 60, 20, 150});
    }
}