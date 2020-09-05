#include "haptics.h"
#include "journal.h"
#include "platform.h"
#include "random.h"
#include "telemetry.h"
#include "window.h"
#include <algorithm>
#include <vector>
#include <ion.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif

/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

class Args {
public:
  Args(int argc, char * argv[]) : m_arguments(argv, argv+argc) {}
  bool has(const char * key) const;
  const char * pop(const char * key);
  bool popFlag(const char * flag);
  void push(const char * key, const char * value) {
    m_arguments.push_back(key);
    m_arguments.push_back(value);
  }
  void push(const char * argument) { m_arguments.push_back(argument); }
  int argc() const { return m_arguments.size(); }
  const char * const * argv() const { return &m_arguments[0]; }
private:
  std::vector<const char *>::const_iterator find(const char * name) const;
  std::vector<const char *> m_arguments;
};

bool Args::has(const char * name) const {
  return find(name) != m_arguments.end();
}

const char * Args::pop(const char * argument) {
  auto nameIt = find(argument);
  if (nameIt != m_arguments.end()) {
    auto valueIt = std::next(nameIt);
    if (valueIt != m_arguments.end()) {
      const char * value = *valueIt;
      m_arguments.erase(valueIt);
      m_arguments.erase(nameIt);
      return value;
    }
    m_arguments.erase(nameIt);
  }
  return nullptr;
}

bool Args::popFlag(const char * argument) {
  auto flagIt = find(argument);
  if (flagIt != m_arguments.end()) {
    m_arguments.erase(flagIt);
    return true;
  }
  return false;
}

std::vector<const char *>::const_iterator Args::find(const char * name) const {
  return std::find_if(
    m_arguments.begin(), m_arguments.end(),
    [name](const char * s) { return strcmp(s, name) == 0; }
  );
}

using namespace Ion::Simulator;

int main(int argc, char * argv[]) {
  Args args(argc, argv);

  if (!args.has("--language")) {
    args.push("--language", Platform::languageCode());
  }

#ifndef __WIN32__
  if (args.popFlag("--limit-stack-usage")) {
    // Limit stack usage
    /* TODO : Reduce stack memory cost in prime factorization to allow running
     * tests with the actual stack size */
    constexpr int kStackSize = 32768*10;
    struct rlimit stackLimits = {kStackSize, kStackSize};
    setrlimit(RLIMIT_STACK, &stackLimits);
  }
#endif

  bool headless = args.popFlag("--headless");

  Random::init();
  if (!headless) {
    Journal::init();
#if EPSILON_TELEMETRY
    Telemetry::init();
#endif
    Window::init();
    Haptics::init();
  }
  ion_main(args.argc(), args.argv());
  if (!headless) {
    Haptics::shutdown();
    Window::quit();
#if EPSILON_TELEMETRY
    Telemetry::shutdown();
#endif
  }

  return 0;
}
