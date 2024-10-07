#include "commands.h"

#include <iostream>

// Command implementations
void simplifyCommand(const std::vector<std::string>& args) {
  // Your simplify logic here
  std::cout << "Simplify command executed with args:";
  for (const auto& arg : args) std::cout << " " << arg;
  std::cout << std::endl;
}

void logCommand(const std::vector<std::string>& args) {
  // Your log logic here
  std::cout << "Log command executed with args:";
  for (const auto& arg : args) std::cout << " " << arg;
  std::cout << std::endl;
}

// Command map
std::map<std::string, void (*)(const std::vector<std::string>&)> commands = {
    {"simplify", simplifyCommand}, {"log", logCommand},
    // Add more commands here
};
