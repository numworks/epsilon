#pragma once

#include <map>
#include <string>
#include <vector>

extern std::map<std::string, void (*)(const std::vector<std::string>&)>
    commands;
extern bool s_isInteractive;
