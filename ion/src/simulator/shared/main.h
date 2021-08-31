#ifndef ION_SIMULATOR_MAIN_H
#define ION_SIMULATOR_MAIN_H

#include <vector>

class Args {
public:
  Args(int argc, char * argv[]) : m_arguments(argv, argv+argc) {}
  bool has(const char * key) const;
  const char * pop(const char * key);
  bool popFlag(const char * flag);
  void push(const char * key, const char * value) {
    if (key != nullptr && value != nullptr) {
      m_arguments.push_back(key);
      m_arguments.push_back(value);
    }
  }
  void push(const char * argument) { m_arguments.push_back(argument); }
  int argc() const { return m_arguments.size(); }
  const char * const * argv() const { return &m_arguments[0]; }
private:
  std::vector<const char *>::const_iterator find(const char * name) const;
  std::vector<const char *> m_arguments;
};

#endif
