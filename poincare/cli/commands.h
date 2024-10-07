#pragma once

#include <map>

extern std::map<std::string, void (*)(const std::vector<std::string>&)>
    commands;
