#pragma once

#include <SDL2/SDL_pixels.h>

enum BlockType
{
    EMPTY = 0,
    WALL,
    DROPPED,
    MOVING
};

struct Vector2
{
    int x;
    int y;
};

struct Block
{
    BlockType Type;
    SDL_Color Color;
    Vector2 Position;
    char Character;
};