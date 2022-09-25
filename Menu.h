#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>

#include "Game.h"

#include "ImGui/imgui.h"

struct ResolutionChoice
{
    std::string title;
    int w, h;
};

class Options;

class Menu
{
 public:
    Menu();
    ~Menu();

    bool Initialize();
    void RunLoop();
    void Shutdown();

    int GetWidth() const;
    int GetHeight() const;
 private:
    void ProcessInput();
    void GenerateOutput();
    bool LoadMenu();
    void UnloadMenu();
    void DrawMenu();

    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    SDL_GLContext glContext;

    bool mIsRunning;
    bool mGameStart = false;

    Uint32 mTicksCount;
    int mWidth;
    int mHeight;

    bool debug = true;

    int mButtonWidth;
    int mButtonHeight;

    ImVec4 clearColor;
    int i, selected_resolution;
    std::vector<ResolutionChoice> resolutions;

    std::unique_ptr<Options> mOptions;
};

