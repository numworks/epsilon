#include "haptics.h"
#include "journal.h"
#include "platform.h"
#include "random.h"
#include "state_file.h"
#include "telemetry.h"
#include "window.h"
#include <algorithm>
#include <ion.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif
#include "store_script.h"
#include <iostream>

/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

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

  bool help = args.popFlag("--help") || args.popFlag("-h");
  bool headless = args.popFlag("--headless");
  bool volatile_storage = args.popFlag("--volatile") || args.popFlag("-v");

#if ION_SIMULATOR_FILES
  const char * stateFile = args.pop("--load-state-file");
  if (stateFile) {
    StateFile::load(stateFile);
  }
#endif

  if (help) {
    std::cout << "Usage: " << argv[0] << "[options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -f, --fullscreen          Starts the emulator in fullscreen" << std::endl;
    std::cout << "  -s, --screen-only         Disable the keyboard." << std::endl;
    std::cout << "  -v, --volatile            Disable saving and loading python scripts from file." << std::endl;
    std::cout << "  -u, --unresizable         Disable resizing the window." << std::endl;
    std::cout << "  -h, --help                Show this help menu." << std::endl;
    return 0;
  }

  Random::init();
  if (!headless) {
    bool screen_only = args.popFlag("--screen-only") || args.popFlag("-s");
    bool fullscreen =  args.popFlag("--fullscreen")  || args.popFlag("-f");
    bool unresizable = args.popFlag("--unresizable") || args.popFlag("-u");
    Journal::init();
#if EPSILON_TELEMETRY
    Telemetry::init();
#endif
    Window::init(screen_only, fullscreen, unresizable);
    Haptics::init();
  }
  if (!volatile_storage) {
    Ion::Simulator::StoreScript::loadPython(&args);
  }
  ion_main(args.argc(), args.argv());
  if (!headless) {
    Haptics::shutdown();
    Window::quit();
#if EPSILON_TELEMETRY
    Telemetry::shutdown();
#endif
  }

  if (!volatile_storage) {
    Ion::Simulator::StoreScript::savePython();
  }

  return 0;
}
