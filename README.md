# Village Builder (Vesnice)

A simulation game built with RayLib and C++ that allows players to build a village from scratch. The game features:

## Core Features

- **Grid-based Building System**: Create paths, zones, walls, and structures on a tile grid
- **NPC Population System**: Manage NPCs who can work, shop, and live in houses
- **Resource Management**: Collect wood and stone resources to build walls and structures
- **Zone Creation**: Define residential (house), commercial (shop), and industrial (sawmill/quarry) zones
- **Wall Building**: Construct stone or wooden walls that protect your village
- **Save/Load System**: Save your progress and load previous game states

## Gameplay Loop

1. **Start Menu**: Choose to start a new game, load a saved game, or access settings
2. **Main Game**: Build your village by drawing paths, creating zones, placing walls
3. **Build Mode**: Select tools from the menu (paths, zones, walls, destroy) and draw on the grid
4. **NPC Management**: Click NPCs to see their details and manage their activities
5. **Resource Gathering**: NPCs can gather wood from trees and stone from rocks
6. **Wall Construction**: NPCs use gathered materials to build walls automatically
7. **Zone Population**: NPCs move into houses when capacity is available
8. **Work System**: NPCs work at sawmills or quarries, bringing resources back home

## Controls

- **Mouse Left Click**: Draw paths, zones, or walls in build mode
- **Mouse Right Click**: Pan camera
- **Mouse Wheel**: Zoom in/out
- **ESC Key**: Pause menu / toggle build modes
- **Left Panel**: Build tools (paths, zones, shops, walls, destroy)
- **Top Bar**: Display money and NPC count
- **Right-click on NPC**: View NPC details

## Technical Details

- Built with RayLib C++ library
- Uses RayGui for UI elements
- Grid-based rendering system
- Camera with zoom and pan support
- Object composition for wall textures (corner pieces, edge pieces)

## File Structure

```
src/
  ├── main.cpp       - Main game loop and UI
  ├── buildings.h/.cpp - Zone management and drawing
  ├── constants.h/.cpp - Texture loading and tile composition
  ├── materials.h/.cpp - Material rendering
  └── npc.h/.cpp      - NPC behavior and movement
```
