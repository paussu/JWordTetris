#include "Hiscores.h"
#include "Menu.h"
#include <fstream>
#include <algorithm>

Hiscores::Hiscores(const Menu *parentMenu)
: mParentMenu(parentMenu), isShowed(false)
{
    LoadFromFile();
}

void Hiscores::Draw()
{
    if (!isShowed)
        return;

    const auto &parentWidth = mParentMenu->GetWidth();
    const auto &parentHeight = mParentMenu->GetHeight();

    ImGui::SetNextWindowSize(ImVec2(parentWidth, parentHeight - (parentHeight / 4.0)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(parentWidth / 4.0, 100));
    ImGui::Begin("Hiscores", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::BeginChild("HiscoreList", ImVec2(400.0, 400.0));
    for (const auto& score : mScoreList)
    {
        ImGui::Text("%-40s %d", score.first.c_str(), score.second);
    }
    ImGui::EndChild();

    if (ImGui::Button("Exit Hiscores", ImVec2(parentWidth  / 4.0, 50)))
        isShowed = false;

    ImGui::End();
}

void Hiscores::SetShown()
{
    LoadFromFile();
    isShowed = true;
}

void Hiscores::LoadFromFile()
{
    mScoreList.clear();

    std::ifstream scoreFile;
    std::string line;

    scoreFile.open("../Hiscores.score");
    if (!scoreFile.is_open())
    {
        SDL_Log("Failed to open hiscores file");
        return;
    }

    std::pair<std::string, int> score;
    while (getline(scoreFile, line, '\n'))
    {
        const auto delimiterPos = line.find(";");
        const auto &playerName = line.substr(0, line.find(";"));
        line.erase(0, delimiterPos + 1);
        const auto score = std::stoi(line);
        mScoreList.push_back({playerName, score});
    }

    std::sort(mScoreList.begin(), mScoreList.end(), [](const std::pair<std::string, int> &score1, const std::pair<std::string, int> &score2)
    {
        return score1.second > score2.second;
    });
}

void Hiscores::SaveScore(const std::string &playerName, int score)
{
    std::ofstream scoreFile;
    std::string line;

    scoreFile.open("../Hiscores.score", std::ios_base::app);
    if (!scoreFile.is_open())
    {
        SDL_Log("Failed to open hiscores file");
        return;
    }

    scoreFile << playerName << ";" << score << std::endl;
}