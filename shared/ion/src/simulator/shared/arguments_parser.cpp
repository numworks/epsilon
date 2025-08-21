#include "arguments_parser.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

bool ArgumentsParser::has(const char* name) const {
  return find(name) != m_arguments.end();
}

const char* ArgumentsParser::get(const char* argument, bool pop) {
  auto nameIt = find(argument);
  if (nameIt != m_arguments.end()) {
    auto valueIt = std::next(nameIt);
    if (valueIt != m_arguments.end()) {
      const char* value = *valueIt;
      if (pop) {
        m_arguments.erase(valueIt);
        m_arguments.erase(nameIt);
      }
      return value;
    }
    if (pop) {
      m_arguments.erase(nameIt);
    }
  }
  return nullptr;
}

const char* ArgumentsParser::pop(const char* const* keys, size_t numberOfKeys) {
  const char* result = nullptr;
  for (size_t i = 0; !result && i < numberOfKeys; i++) {
    result = pop(keys[i]);
  }
  return result;
}

bool ArgumentsParser::popFlag(const char* argument) {
  auto flagIt = find(argument);
  if (flagIt != m_arguments.end()) {
    m_arguments.erase(flagIt);
    return true;
  }
  return false;
}

bool ArgumentsParser::popFlags(const char* const* flags, size_t numberOfFlags) {
  bool result = false;
  for (size_t i = 0; !result && i < numberOfFlags; i++) {
    result = popFlag(flags[i]);
  }
  return result;
}

std::vector<const char*>::const_iterator ArgumentsParser::find(
    const char* name) const {
  return std::find_if(m_arguments.begin(), m_arguments.end(),
                      [name](const char* s) { return strcmp(s, name) == 0; });
}

Arguments ArgumentsParser::parse() {
  Arguments result;
#define BOOL_ARG(variable, longForm, shortForm, desc) \
  result.variable =                                   \
      popFlag(longForm) || (std::strlen(shortForm) && popFlag(shortForm));
#define TEXT_ARG(variable, longForm, shortForm, desc, argName) \
  result.variable = pop(longForm);                             \
  if (!result.variable && std::strlen(shortForm))              \
    result.variable = pop(shortForm);
#include "arguments.inc"
  return result;
}

void ArgumentsParser::printArgument(const char* longForm, const char* shortForm,
                                    const char* description,
                                    const char* argName) {
  std::string arg;
  arg += longForm;
  if (std::strlen(shortForm)) {
    arg += '|';
    arg += shortForm;
  }
  if (argName) {
    arg += ' ';
    arg += argName;
  }
  std::cout << "  " << std::setw(40) << std::left << arg << description << '\n';
}

void ArgumentsParser::printHelp() {
  std::cout << "Arguments:\n";
#define BOOL_ARG(variable, longForm, shortForm, desc) \
  printArgument(longForm, shortForm, desc, nullptr);
#define TEXT_ARG(variable, longForm, shortForm, desc, argName) \
  printArgument(longForm, shortForm, desc, argName);
#include "arguments.inc"
}
