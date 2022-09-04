
#include "Menu.h"
#include "Game.h"
#include "Options.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

Menu::Menu()
{
    clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    mOptions = std::make_unique<Options>(this);
}

Menu::~Menu() = default;

bool Menu::Initialize()
{
    mWidth = 1024;
    mHeight = 768;

    if(!LoadMenu()) 
        return false;

    mIsRunning = true;
    mGameStart = false;

    mButtonWidth = 320;
    mButtonHeight = 120;
    selected_resolution = 2;

    resolutions.push_back({"640x480", 640, 480});
    resolutions.push_back({"800x600", 800, 600});
    resolutions.push_back({"1024x768", 1024, 768});
    resolutions.push_back({"1280×960", 1280, 960});
    resolutions.push_back({"1440x900", 1440, 900});
    resolutions.push_back({"1600×1200", 1600, 1200});
    resolutions.push_back({"1920x1080", 1920, 1080});

    return true;
}

void Menu::Shutdown()
{
    UnloadMenu();
}

void Menu::RunLoop()
{
    while(mIsRunning)
    {
        ProcessInput();

        if(mGameStart)
        {
            UnloadMenu();
            auto game = std::make_unique<Game>(&mOptions->GetGameConfiguration());
            bool success = game->Initialize();

            if(success)
            {
                game->Run();
            }

            game->Shutdown();
            mGameStart = false;
            LoadMenu();
        }

        GenerateOutput();
    }
}

bool Menu::LoadMenu()
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

    int sdlResult = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
    if(sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL! Error was: %s", SDL_GetError());
        return false;
    }

    // GL 3.0 + GLSL 130
    const char* glslVersion = "#version 130";
     SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    mWindow = SDL_CreateWindow(mOptions->GetGameConfiguration().title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight,
                               windowFlags);

    if(!mWindow)
    {
        SDL_Log("Unable to create window! Error was %s", SDL_GetError());
        return false;
    }

    glContext = SDL_GL_CreateContext(mWindow);
    SDL_GL_MakeCurrent(mWindow, glContext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(mWindow, glContext);
    ImGui_ImplOpenGL3_Init(glslVersion);

    if (!glContext)
    {
        SDL_Log("Failed to create OpenGL context! Error was %s", SDL_GetError());
        return false;
    }

    return true;
}

void Menu::UnloadMenu()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Menu::ProcessInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            mIsRunning = false;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(mWindow))
            mIsRunning = false;
    }
}

void Menu::GenerateOutput()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    DrawMenu();
    mOptions->Draw();

    // Rendering
    ImGui::Render();
    glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(mWindow);
}

void Menu::DrawMenu()
{
    ImGui::SetNextWindowSize(ImVec2(mWidth  / 4.0, mHeight - (mHeight / 2.0)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(mWidth / 3.0, 100));
    ImGui::Begin("Start menu", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Play", ImVec2(mWidth  / 4.0, 100)))
        mGameStart = true;

    if (ImGui::Button("Options", ImVec2(mWidth  / 4.0, 100)))
        mOptions->SetShown();

    if (ImGui::Button("Hiscores", ImVec2(mWidth  / 4.0, 100)))
        auto todo = true;

    if (ImGui::Button("Exit", ImVec2(mWidth  / 4.0, 100)))
        mIsRunning = false;

    ImGui::End();
}

int Menu::GetWidth() const
{
    return mWidth;
}

int Menu::GetHeight() const
{
    return mHeight;
}