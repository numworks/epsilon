#pragma once

#include <cstdint>
#include <vector>

#include "arguments.h"
/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

class ArgumentsParser {
 public:
  ArgumentsParser(int argc, char* argv[]) : m_arguments(argv, argv + argc) {}
  Arguments parse();
  void printHelp();

 private:
  bool has(const char* key) const;
  const char* get(const char* key, bool pop = false);
  const char* pop(const char* key) { return get(key, true); }
  const char* pop(const char* const* keys, size_t numberOfKeys);
  bool popFlag(const char* flag);
  bool popFlags(const char* const* flags, size_t numberOfFlags);
  void printArgument(const char* longForm, const char* shortForm,
                     const char* description, const char* argName);

 public:
  void push(const char* key, const char* value) {
    if (key != nullptr && value != nullptr) {
      m_arguments.push_back(key);
      m_arguments.push_back(value);
    }
  }

  void push(const char* argument) { m_arguments.push_back(argument); }
  int argc() const { return m_arguments.size(); }
  const char* const* argv() const { return &m_arguments[0]; }

 private:
  std::vector<const char*>::const_iterator find(const char* name) const;
  std::vector<const char*> m_arguments;
};
