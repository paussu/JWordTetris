#include "Game.h"
#include "Options.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <utility>

Game::Game(const GameConfiguration* config)
:
mConfiguration(config),
mGameMap(mMapWidth, std::vector<Block>(mMapHeight, {WALL, SDL_Color{255, 255, 255, 255}, '?'}))
{
    SDL_assert(config);

    mBlockSize = mConfiguration->screenWidth / 64;
    characterDistribution = std::uniform_int_distribution<std::mt19937::result_type>(0, mConfiguration->blockCharset.size() - 1);
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL! Error was: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(mConfiguration->title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mConfiguration->screenWidth, mConfiguration->screenHeight, 0);

    if (!window)
    {
        SDL_Log("Unable to create window! Error was: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!mRenderer)
    {
        SDL_Log("Unable to create a renderer! Error was: %s", SDL_GetError());
        return false;
    }

    TTF_Init();

    for (int x = 2; x < mMapWidth - 2; x++)
    {
        for (int y = 0; y < mMapHeight - 2; y++)
        {
            mGameMap[x][y].Type = EMPTY;
        }
    }

    mFont = TTF_OpenFont("../Assets/PressStart2P-Regular.ttf", 24);
    if (!mFont)
    {
        SDL_Log("Failed to open font\n");
        return false;
    }

    std::ifstream wordFile;
    std::string line;

    wordFile.open(mConfiguration->wordListFile);
    if (!wordFile.is_open())
    {
        SDL_Log("Failed to open word list file");
        return false;
    }

    while (std::getline(wordFile, line))
    {
        // Make sure these are not getting in
        std::erase(line, '\r');
        std::erase(line, '\n');

        mWordList.push_back(line);
    }

    return true;
}

void Game::Run()
{
    while (isRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown()
{
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_a:
                    case SDLK_LEFT:
                        UpdatePosition(-1, 0);
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        UpdatePosition(1, 0);
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        UpdatePosition(0, 1);
                        break;
                    case SDLK_p:
                        gameStopped = !gameStopped;
                        break;
                    case SDLK_h:
                        hideKeyboardInfo = !hideKeyboardInfo;
                        break;
                    case SDLK_SPACE:
                        for (int i = 0; i < mMapHeight - 2; i++)
                        {
                            if (!UpdatePosition(0, 1))
                                break;
                        }
                        UpdateBlocks();
                        break;
                    case SDLK_RETURN:
                        InsertBlock();
                        hideKeyboardInfo = true;
                        break;
                }
                break;
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    if (state[SDL_SCANCODE_ESCAPE])
    {
        isRunning = false;
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16))
        continue;

    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f;

    // Clamp maximum delta value
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    currentTime = ticksCount = SDL_GetTicks();

    if (currentTime > lastTime + mDropSpeed)
    {
        UpdateBlocks();
        lastTime = currentTime;
    }
}

void Game::GenerateOutput() const
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    DrawDebugStuff();
    DrawMap();
    DrawWordList();
    DrawKeyboardInfo();

    std::string scoreText = "SCORE: " + std::to_string(mScore);
    RenderText(scoreText.c_str(), mConfiguration->screenWidth / 2 + mBlockSize, 100 , mConfiguration->screenWidth / 4, mConfiguration->screenHeight / 20);

    SDL_RenderPresent(mRenderer);
}

void Game::InsertBlock()
{
    if (isBlock || gameStopped)
        return;

    SDL_Color blockColor = {25, 25, 255, 255};
    mBlock = std::make_unique<Block>();
    mBlock->Type = BlockType::MOVING;
    mBlock->Color = blockColor;
    mBlock->Character = GetRandomCharacter();
    mBlock->Position.x = 10;
    mBlock->Position.y = 0;

    isBlock = true;
}

void Game::UpdateBlocks()
{
    if (mBlock == nullptr || gameStopped)
        return;

    std::vector<Vector2> wordPositions;
    for (int y = 0; y < mMapHeight - 1; y++)
    {
        for (int x = 1; x < mMapWidth - 1; x++)
        {
            if (mGameMap[x][y].Type == DROPPED && mGameMap[x][y + 1].Type == EMPTY)
            {
                std::swap(mGameMap[x][y + 1], mGameMap[x][y]);
                return;
            }
            if (mGameMap[x][2].Type == DROPPED)
            {
                isRunning = false;
                return;
            }
            if (mGameMap[x][y].Type != DROPPED)
            {
                mGameMap[x][y].Type = EMPTY;
            }
            wordPositions = CheckForWords(x, y);
            if (!wordPositions.empty())
                break;
        }
        if (!wordPositions.empty())
        {
            AddScore(wordPositions.size());
            for (const auto& wordPosition : wordPositions)
            {
                mGameMap[wordPosition.x][wordPosition.y].Type = EMPTY;
                mGameMap[wordPosition.x][wordPosition.y].Character = 0;
            }
        }
    }

    ApplyBlockToMap(MOVING);

    UpdatePosition(0, 1);
}

bool Game::UpdatePosition(int x, int y)
{
    if (mBlock == nullptr || gameStopped)
        return false;

    if (mBlock->Type == MOVING)
    {
        int xpos = mBlock->Position.x + x;
        int ypos = mBlock->Position.y + y;

        if (mGameMap[xpos][ypos].Type == WALL || mGameMap[xpos][ypos].Type == DROPPED)
        {
            if (y > 0)
            {
                ApplyBlockToMap(DROPPED);
                mBlock = nullptr;
                isBlock = false;
                InsertBlock();
            }

            return false;
        }
    }

    mBlock->Position.x += x;
    mBlock->Position.y += y;
    return true;
}

void Game::ApplyBlockToMap(BlockType valueToApply)
{
    if (mBlock->Type == MOVING)
    {
        mGameMap[mBlock->Position.x][mBlock->Position.y].Type = valueToApply;
        mGameMap[mBlock->Position.x][mBlock->Position.y].Color = mBlock->Color;
        mGameMap[mBlock->Position.x][mBlock->Position.y].Character = mBlock->Character;
    }
}

void Game::RenderText(std::string_view text, int x, int y, int w, int h) const
{
    SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, text.data(), {255, 255, 255, 255});
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);

    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = w;
    textRect.h = h;

    SDL_RenderCopy(mRenderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::EmptyMap()
{
    for (int y = 0; y < mMapHeight - 1; y++)
    {
        for (int x = 1; x < mMapWidth - 1; x++)
        {
            mGameMap[x][y].Type = EMPTY;
        }
    }
}

void Game::AddScore(int wordLength)
{
    mScore += 10 * mLevel * wordLength;
    mWordCount++;

    if (mWordCount % 10 == 0)
    {
        mLevel++;
        mDropSpeed -= 10;
    }
}

std::vector<Vector2> Game::CheckForWords(int x, int y)
{
    std::vector<Vector2> wordPositions;

    std::string word;
    for (int xpos = x; xpos < mMapWidth - 1; xpos++)
    {
        const auto& block = mGameMap[xpos][y];

        if (block.Type != DROPPED)
            break;

        word.push_back(block.Character);
        wordPositions.emplace_back(Vector2{xpos, y});
        if (std::binary_search(mWordList.begin(), mWordList.end(), word))
        {
            int score = word.length() * 10;
            mWordsFound.emplace_back(word, score);
            if(mWordsFound.size() > 8)
                mWordsFound.pop_front();
            return wordPositions;
        }
    }

    wordPositions.clear();
    word.clear();
    for (int ypos = y; ypos < mMapHeight - 1; ypos++)
    {
        const auto& block = mGameMap[x][ypos];

        if (block.Type == EMPTY || block.Type == WALL)
            break;

        word.push_back(block.Character);
        wordPositions.emplace_back(Vector2{x, ypos});
        if (std::binary_search(mWordList.begin(), mWordList.end(), word))
        {
            int score = word.length() * 10;
            mWordsFound.emplace_back(word, score);
            if(mWordsFound.size() > 8)
                mWordsFound.pop_front();
            return wordPositions;
        }
    }

    return {};
}

char Game::GetRandomCharacter()
{
    return mConfiguration->blockCharset[characterDistribution(mRandomNumberGenerator)];
}

void Game::DrawMap() const
{
    const int xOffset = mBlockSize * 6;
    const int yOffset = mBlockSize * 6;
    // Left vertical line
    SDL_RenderDrawLine(mRenderer, xOffset + mBlockSize, yOffset, xOffset + mBlockSize, yOffset + mMapHeight * mBlockSize + mBlockSize);
    // Right vertical line
    SDL_RenderDrawLine(mRenderer, xOffset + (mBlockSize * mMapWidth), yOffset, xOffset + (mBlockSize * mMapWidth), yOffset + mMapHeight * mBlockSize + mBlockSize);
    // Horizontal line
    SDL_RenderDrawLine(mRenderer, xOffset + mBlockSize, yOffset + (mBlockSize * mMapHeight) + mBlockSize, xOffset + (mBlockSize * mMapWidth), yOffset + (mBlockSize * mMapHeight) + mBlockSize);

    for (int x = 0; x < mMapWidth; x++)
    {
        for (int y = 0; y < mMapHeight; y++)
        {
            const auto& block = mGameMap[x][y];
            if (block.Type == MOVING || block.Type == DROPPED)
            {
                SDL_Rect mapRect;
                mapRect.x = xOffset + x * (mBlockSize + mBlockGap);
                mapRect.y = yOffset + y * (mBlockSize + mBlockGap);
                mapRect.h = mBlockSize;
                mapRect.w = mBlockSize;

                SDL_SetRenderDrawColor(mRenderer, block.Color.r, block.Color.g, block.Color.b, block.Color.a);
                SDL_RenderFillRect(mRenderer, &mapRect);
                RenderText(std::string(1, block.Character), mapRect.x, mapRect.y, mapRect.w, mapRect.h);
            }
        }
    }
}

void Game::DrawWordList() const
{
    SDL_Rect wordBox;
    wordBox.x = mConfiguration->screenWidth / 2 + (mBlockSize * 2);
    wordBox.y = mBlockSize * 12;
    wordBox.w = mBlockSize * 12;
    wordBox.h = mBlockSize * 12;
    SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(mRenderer, &wordBox);
    RenderText("Words found: " + std::to_string(mWordsFound.size()), wordBox.x, wordBox.y - 30, wordBox.w, 20);

    int wordY = wordBox.y;
    int characterSize = wordBox.w / mConfiguration->maxWordLength;
    int padding = 2;
    for(const auto &[word, score] : mWordsFound)
    {
        // TODO: Format with something nicer
        char buffer [32];
        const auto scoreText = std::to_string(score);
        sprintf(buffer, "%s%*s%s", word.c_str(), static_cast<int>(mConfiguration->maxWordLength - word.length() - scoreText.length() + 1) ,"",  scoreText.c_str());
        std::string wordScoreText(buffer);
        const auto &wordLength = wordScoreText.length();
        RenderText(wordScoreText, wordBox.x + padding, wordY + padding, wordLength * characterSize, wordBox.h / 8 - padding);
        wordY += wordBox.h / 8;
    }
}

void Game::DrawKeyboardInfo() const
{
    if (!hideKeyboardInfo)
    {
        RenderText("Use wasd or arrow keys for movement",
            mConfiguration->screenWidth / 2 + mBlockSize,
            mConfiguration->screenHeight / 2 + mBlockSize,
            mConfiguration->screenWidth / 3,
            mConfiguration->screenHeight / 16);
        RenderText("Press p to pause game",
            mConfiguration->screenWidth / 2 + mBlockSize,
            mConfiguration->screenHeight / 2 + mBlockSize * 6,
            mConfiguration->screenWidth / 3,
            mConfiguration->screenHeight / 16);
        RenderText("Press enter to start game",
            mConfiguration->screenWidth / 2 + mBlockSize,
            mConfiguration->screenHeight / 2 + mBlockSize * 12,
            mConfiguration->screenWidth / 3,
            mConfiguration->screenHeight / 16);
    }
}

void Game::DrawDebugStuff() const
{
    if(!mConfiguration->debug)
        return;

    SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
    for(int x = 0; x < mConfiguration->screenWidth; x += mBlockSize)
    {
        if(x > mConfiguration->screenWidth / 2)
            SDL_SetRenderDrawColor(mRenderer, 120, 0, 0, 255);

        SDL_RenderDrawLine(mRenderer, x, 0, x, mConfiguration->screenHeight);
        for(int y = 0; y < mConfiguration->screenHeight; y += mBlockSize)
        {
            SDL_RenderDrawLine(mRenderer, 0, y, mConfiguration->screenWidth, y);
        }
    }
}

int Game::GetScore()
{
    return mScore;
}