#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <bits/unique_ptr.h>
#include <cmath>

#include <random>
#include <vector>
#include <deque>
#include <string>
#include <memory>

#include "Block.h"

struct GameConfiguration;
struct WordScore
{
   std::string word;
   int score;
};

class Game
{
 public:
    explicit Game(const GameConfiguration* config);

    bool Initialize();
    void Run();
    void Shutdown();

 private:
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput() const;
    void RestartGame();

    void AddScore(int wordLength);
    void InsertBlock();
    void UpdateBlocks();
    void ApplyBlockToMap(BlockType valueToApply);
    void EmptyMap();
    char GetRandomCharacter();
    bool UpdatePosition(int x, int y);
    std::vector<Vector2> CheckForWords(int x, int y);
    void RenderText(std::string_view text, int x, int y, int w, int h) const;
    void DrawMap() const;
    void DrawWordList() const;
    void DrawKeyboardInfo() const;

    void DrawDebugStuff() const;

    const GameConfiguration* mConfiguration {nullptr};
    SDL_Window* window;
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;

    bool isRunning {true};
    bool isBlock {false};
    bool gameRestarted {false};
    bool gameStopped {false};
    bool hideKeyboardInfo {false};

    const int mMapWidth {20};
    const int mMapHeight {30};
    const int mBlockGap {1};

    int mBlockSize;
    int mDropSpeed {500};
    int mScore {0};
    int mWordCount {0};
    int mLevel {1};

    std::string LevelText;
    std::string LinesText;
    std::vector<std::string> mWordList;

    Uint32 ticksCount {0};
    Uint32 currentTime {0};
    Uint32 lastTime {0};

    std::random_device mRandomDevice;
    std::mt19937 mRandomNumberGenerator {mRandomDevice()};
    std::uniform_int_distribution<std::mt19937::result_type> characterDistribution;

    std::vector<std::vector<Block>> mGameMap;
    std::unique_ptr<Block> mBlock;

    std::deque<WordScore> mWordsFound;
};

