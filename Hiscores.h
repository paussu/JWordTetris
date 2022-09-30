#pragma once

#include <vector>
#include <string>

class Menu;

class Hiscores
{
 public:
    explicit Hiscores(const Menu* parentMenu);
    ~Hiscores() = default;

    void Draw();
    void SetShown();
    void SaveScore(const std::string &playerName, int score);
    void LoadFromFile();

 private:
    const Menu* mParentMenu;
    bool isShowed;
    std::vector<std::pair<std::string, int>> mScoreList;
};