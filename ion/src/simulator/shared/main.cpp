#include <assert.h>
#include <ion.h>
#include <ion/src/shared/init.h>

#include <algorithm>
#include <vector>

#include "haptics.h"
#include "journal.h"
#include "platform.h"
#include "random.h"
#include "state_file.h"
#include "telemetry.h"
#include "window.h"
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif
#if ION_SIMULATOR_FILES
#include <signal.h>
#include <stdio.h>

#include "actions.h"
#include "screenshot.h"
extern "C" {
extern char *eadk_external_data;
extern size_t eadk_external_data_size;
}
#include <dlfcn.h>
#endif

constexpr static const char *k_loadStateFileKeys[] = {"--load-state-file",
                                                      "-l"};
constexpr static const char *k_headlessFlags[] = {"--headless", "-h"};

/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

class Args {
 public:
  Args(int argc, char *argv[]) : m_arguments(argv, argv + argc) {}
  bool has(const char *key) const;
  const char *get(const char *key, bool pop = false);
  const char *pop(const char *key) { return get(key, true); }
  const char *pop(const char *const *keys, size_t numberOfKeys);
  bool popFlag(const char *flag);
  bool popFlags(const char *const *flags, size_t numberOfFlags);
  void push(const char *key, const char *value) {
    if (key != nullptr && value != nullptr) {
      m_arguments.push_back(key);
      m_arguments.push_back(value);
    }
  }
  void push(const char *argument) { m_arguments.push_back(argument); }
  int argc() const { return m_arguments.size(); }
  const char *const *argv() const { return &m_arguments[0]; }

 private:
  std::vector<const char *>::const_iterator find(const char *name) const;
  std::vector<const char *> m_arguments;
};

bool Args::has(const char *name) const {
  return find(name) != m_arguments.end();
}

const char *Args::get(const char *argument, bool pop) {
  auto nameIt = find(argument);
  if (nameIt != m_arguments.end()) {
    auto valueIt = std::next(nameIt);
    if (valueIt != m_arguments.end()) {
      const char *value = *valueIt;
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

const char *Args::pop(const char *const *keys, size_t numberOfKeys) {
  const char *result = nullptr;
  for (int i = 0; !result && i < numberOfKeys; i++) {
    result = pop(keys[i]);
  }
  return result;
}

bool Args::popFlag(const char *argument) {
  auto flagIt = find(argument);
  if (flagIt != m_arguments.end()) {
    m_arguments.erase(flagIt);
    return true;
  }
  return false;
}

bool Args::popFlags(const char *const *flags, size_t numberOfFlags) {
  bool result = false;
  for (int i = 0; !result && i < numberOfFlags; i++) {
    result = popFlag(flags[i]);
  }
  return result;
}

std::vector<const char *>::const_iterator Args::find(const char *name) const {
  return std::find_if(m_arguments.begin(), m_arguments.end(),
                      [name](const char *s) { return strcmp(s, name) == 0; });
}

#if ION_SIMULATOR_FILES
static inline int load_eadk_external_data(const char *path) {
  if (path == nullptr) {
    return 0;
  }
  if (eadk_external_data != nullptr || eadk_external_data_size != 0) {
    fprintf(stderr, "Warning: eadk_external_data already loaded\n");
    return -1;
  }
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error loading external data file %s\n", path);
    return -1;
  }
  fseek(f, 0, SEEK_END);
  eadk_external_data_size = ftell(f);
  fseek(f, 0, SEEK_SET);
  eadk_external_data = static_cast<char *>(malloc(eadk_external_data_size));
  if (eadk_external_data == nullptr) {
    fprintf(stderr, "Error allocating external data file\n");
    fclose(f);
    return -1;
  }
  size_t ret = fread(eadk_external_data, 1, eadk_external_data_size, f);
  if (ret != eadk_external_data_size) {
    printf("Error reading %s into eadk_external_data\n", path);
    fclose(f);
    return -1;
  }
  fclose(f);
  return 0;
}
#endif

using namespace Ion::Simulator;

int main(int argc, char *argv[]) {
  Args args(argc, argv);

#ifndef __WIN32__
  if (args.popFlag("--limit-stack-usage")) {
    // Limit stack usage
    constexpr int kStackSize = 32768;
    struct rlimit stackLimits = {kStackSize, kStackSize};
    setrlimit(RLIMIT_STACK, &stackLimits);
  }
#endif

#if ION_SIMULATOR_FILES
  const char *stateFile = args.pop(
      k_loadStateFileKeys, sizeof(k_loadStateFileKeys) / sizeof(char *));
  if (stateFile) {
    assert(Journal::replayJournal());
    StateFile::load(stateFile);
    const char *replayJournalLanguage =
        Journal::replayJournal()->startingLanguage();
    if (replayJournalLanguage[0] != 0) {
      // Override any language setting if there is
      args.pop("--language");
      args.push("--language", replayJournalLanguage);
    }
  }

  const char *screenshotPath = args.pop("--take-screenshot");
  if (screenshotPath) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(screenshotPath);
  }

  const char *allScreenshotsFolder = args.pop("--take-all-screenshots");
  if (allScreenshotsFolder) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->initEachStep(
        allScreenshotsFolder);
  }
#if !defined(_WIN32)
  signal(SIGUSR1, Ion::Simulator::Actions::handleUSR1Sig);
#endif
#endif

  // Default language
  if (!args.has("--language")) {
    args.push("--language", Platform::languageCode());
  }

  bool headless =
      args.popFlags(k_headlessFlags, sizeof(k_headlessFlags) / sizeof(char *));

  Random::init();
  if (!headless) {
    Journal::init();
    if (args.has("--language") && Journal::logJournal()) {
      // Set log journal starting language
      Journal::logJournal()->setStartingLanguage(args.get("--language"));
    }
#if EPSILON_TELEMETRY
    Telemetry::init();
#endif
    Window::init();
    Haptics::init();
  }

#if ION_SIMULATOR_FILES
  const char *nwb = args.pop("--nwb");
  if (nwb) {
    if (load_eadk_external_data(args.pop("--nwb-external-data"))) {
      return -1;
    }
    void *handle = dlopen(nwb, RTLD_LAZY);
    if (handle == nullptr) {
      fprintf(stderr, "Error loading %s: %s\n", nwb, dlerror());
      return -1;
    }
    int (*nwb_main)(int argc, const char *const argv[]);
    *(void **)(&nwb_main) = dlsym(handle, "main");
    if (nwb_main == nullptr) {
      fprintf(stderr, "Could not locate nwb_main symbol: %s\n", dlerror());
      return -1;
    }
    nwb_main(args.argc(), args.argv());
    dlclose(handle);
  } else {
#endif
    Ion::Init();
    ion_main(args.argc(), args.argv());
#if ION_SIMULATOR_FILES
  }
#endif
  if (!headless) {
    Haptics::shutdown();
    Window::shutdown();
#if EPSILON_TELEMETRY
    Telemetry::shutdown();
#endif
  }

  return 0;
}
