//modify at 28-03-2026 by Lancelot
#pragma once
#include <vector>
#include <string>
using namespace std;

struct GameObject {
    int id;
    string name;
    float position[2];
    float scale[2];
};

struct SceneState {
    std::vector<GameObject> objects;
};