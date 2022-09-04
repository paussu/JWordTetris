#pragma once

#include <vector>
#include <SDL2/SDL_stdinc.h>
#include "Block.h"

enum TetrominoType
{
    I = 0,
    O,
    T,
    J,
    L,
    S,
    Z
};

class Tetromino
{
public:
    Tetromino(TetrominoType type, SDL_Color color);
    std::vector<std::vector<Block>> Blocks;
    Vector2 Position;
    void AddRotation();
    int GetRotation();
    TetrominoType GetType();
private:
    int Rotation;
    TetrominoType Type;
};