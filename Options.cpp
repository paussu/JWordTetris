#include "Options.h"
#include "Menu.h"

Options::Options(const Menu *parentMenu)
: mParentMenu(parentMenu), mGameConfiguration{1024, 768, false, true, "JWordTetris"}
, comboItems{"800x600", "1024x768", "1280x720", "1440x900", "1600x900", "1920x1080", "2560x1440"}
{
}

void Options::Draw()
{
    if (!isShowed)
        return;

    const auto &parentWidth = mParentMenu->GetWidth();
    const auto &parentHeight = mParentMenu->GetHeight();

    ImGui::SetNextWindowSize(ImVec2(parentWidth, parentHeight - (parentHeight / 4.0)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(parentWidth / 4.0, 100));
    ImGui::Begin("Options", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Combo("Resolution", &mSelectedResolution, comboItems, IM_ARRAYSIZE(comboItems));
    ImGui::Checkbox("Fullscreen", &mGameConfiguration.fullscreen);
    ImGui::Checkbox("Use Sprites", &mGameConfiguration.useSprites);
    if (ImGui::Button("Exit Options", ImVec2(parentWidth  / 4.0, 50)))
        isShowed = false;

    ImGui::End();
}

void Options::SetShown()
{
    isShowed = true;
}

const GameConfiguration& Options::GetGameConfiguration()
{
    switch (mSelectedResolution)
    {
        case GameResolution::Resolution_800x600:
            mGameConfiguration.screenWidth = 800;
            mGameConfiguration.screenHeight = 600;
            break;
        case GameResolution::Resolution_1024x768:
            mGameConfiguration.screenWidth = 1024;
            mGameConfiguration.screenHeight = 768;
            break;
        case GameResolution::Resolution_1280x720:
            mGameConfiguration.screenWidth = 1280;
            mGameConfiguration.screenHeight = 720;
            break;
        case GameResolution::Resolution_1440x900:
            mGameConfiguration.screenWidth = 1440;
            mGameConfiguration.screenHeight = 900;
            break;
        case GameResolution::Resolution_1600x900:
            mGameConfiguration.screenWidth = 1600;
            mGameConfiguration.screenHeight = 900;
            break;
        case GameResolution::Resolution_1920x1080:
            mGameConfiguration.screenWidth = 1920;
            mGameConfiguration.screenHeight = 1080;
            break;
        case GameResolution::Resolution_2560x1440:
            mGameConfiguration.screenWidth = 2560;
            mGameConfiguration.screenHeight = 1440;
            break;
        default:
            mGameConfiguration.screenWidth = 1024;
            mGameConfiguration.screenHeight = 768;
            break;
    }
    return mGameConfiguration;
}