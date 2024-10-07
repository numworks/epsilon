#include <editline/readline.h>
#include <ion/src/shared/init.h>
#include <ion/src/simulator/shared/random.h>
#include <poincare/init.h>
#include <pwd.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "commands.h"

std::vector<std::string> commandNames;

void initializeCommands() {
  for (const auto& cmd : commands) {
    commandNames.push_back(cmd.first);
  }
}

std::string matchCommand(const std::string& input) {
  std::vector<std::string> matches;
  for (const auto& cmd : commands) {
    if (cmd.first.find(input) == 0) {  // Starts with input
      matches.push_back(cmd.first);
    }
  }

  if (matches.size() == 1) {
    return matches[0];
  } else if (matches.empty()) {
    return "";
  } else {
    std::cerr << "Ambiguous command: " << input << std::endl;
    return "";
  }
}

char* completionGenerator(const char* text, int state) {
  static size_t list_index;
  static size_t len;
  if (state == 0) {
    list_index = 0;
    len = std::strlen(text);
  }

  while (list_index < commandNames.size()) {
    const std::string& cmd = commandNames[list_index];
    list_index++;
    if (cmd.compare(0, len, text) == 0) {
      return strdup(cmd.c_str());
    }
  }

  return nullptr;
}

char** completionFunction(const char* text, int start, int end) {
  if (start == 0) {  // Only complete the first word
    return rl_completion_matches(text, completionGenerator);
  } else {
    return nullptr;  // No completion for arguments
  }
}

void setupCompletion() {
  rl_attempted_completion_function = completionFunction;
}

std::string getHomeDirectory() {
  const char* homedir;
  if ((homedir = getenv("HOME")) == nullptr) {
    homedir = getpwuid(getuid())->pw_dir;
  }
  return std::string(homedir);
}

void initializeHistory() {
  std::string historyFile = getHomeDirectory() + "/.poincare_history";
  read_history(historyFile.c_str());
}

void saveHistory() {
  std::string historyFile = getHomeDirectory() + "/.poincare_history";
  write_history(historyFile.c_str());
}

int main() {
  bool isInteractive = isatty(STDIN_FILENO);

  initializeCommands();

  if (isInteractive) {
    setupCompletion();
    initializeHistory();
  }

  Ion::Simulator::Random::init();
  Ion::Init();
  Poincare::Init();

  std::string prompt = "> ";

  while (true) {
    char* input = readline(prompt.c_str());
    if (input == nullptr) {  // EOF (e.g., Ctrl+D)
      if (isInteractive) {
        std::cout << "\nExiting." << std::endl;
      }
      break;
    }

    std::string line(input);
    free(input);  // libedit allocates input with malloc

    size_t first = line.find_first_not_of(" \t");
    size_t last = line.find_last_not_of(" \t");
    if (first == std::string::npos) continue;  // Empty line
    line = line.substr(first, (last - first + 1));

    add_history(line.c_str());

    // Tokenize input
    std::vector<std::string> tokens;
    size_t pos = 0;
    while ((pos = line.find(' ')) != std::string::npos) {
      tokens.push_back(line.substr(0, pos));
      line.erase(0, pos + 1);
    }
    tokens.push_back(line);  // Last token

    if (tokens.empty()) continue;

    // Match command with possible abbreviation
    std::string cmdName = matchCommand(tokens[0]);
    if (cmdName.empty()) {
      std::cerr << "unknown command " << tokens[0] << '\n';
      continue;
    }

    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    commands[cmdName](args);
  }

  if (isInteractive) {
    saveHistory();
  }

  return 0;
}
