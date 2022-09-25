#pragma once

#include <string>

struct GameConfiguration
{
    int screenWidth;
    int screenHeight;
    int maxWordLength {15};
    bool fullscreen;
    bool debug;
    std::string title;
    std::string blockCharset;
    std::string wordListFile;
};

enum GameResolution
{
    Resolution_800x600 = 0,
    Resolution_1024x768,
    Resolution_1280x720,
    Resolution_1440x900,
    Resolution_1600x900,
    Resolution_1920x1080,
    Resolution_2560x1440
};

class Options
{
 public:
    explicit Options(const class Menu* parentMenu);
    ~Options() = default;

    void Draw();
    void SetShown();

    [[nodiscard]] const GameConfiguration& GetGameConfiguration();

 private:
    const class Menu* mParentMenu;
    bool isShowed {false};
    int mSelectedResolution {1};
    const char* comboItems[7];

    GameConfiguration mGameConfiguration;
};

