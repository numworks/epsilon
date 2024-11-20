#include <assert.h>
#include <ion.h>
#include <unistd.h>

#if ION_LOG_EVENTS_NAME
#include <ion/keyboard/layout_events.h>
#endif
#include <ion/src/shared/init.h>

#include <algorithm>
#include <array>
#include <vector>

#include "haptics.h"
#include "journal.h"
#include "platform.h"
#include "random.h"
#include "state_file.h"
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
#endif

#if ION_SIMULATOR_EXTERNAL_APP
extern "C" {
extern char* eadk_external_data;
extern size_t eadk_external_data_size;
}
#include <dlfcn.h>
#endif

constexpr static const char* k_loadStateFileKeys[] = {"--load-state-file",
                                                      "-l"};
constexpr static const char* k_headlessFlags[] = {"--headless", "-h"};
constexpr static const char* k_languageFlag = "--language";
constexpr static const char* k_limitStackUsageFlag = "--limit-stack-usage";

/* The Args class allows parsing and editing command-line arguments
 * The editing part allows us to add/remove arguments before forwarding them to
 * ion_main. */

class Args {
 public:
  Args(int argc, char* argv[]) : m_arguments(argv, argv + argc) {}
  bool has(const char* key) const;
  const char* get(const char* key, bool pop = false);
  const char* pop(const char* key) { return get(key, true); }
  const char* pop(const char* const* keys, size_t numberOfKeys);
  bool popFlag(const char* flag);
  bool popFlags(const char* const* flags, size_t numberOfFlags);
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

bool Args::has(const char* name) const {
  return find(name) != m_arguments.end();
}

const char* Args::get(const char* argument, bool pop) {
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

const char* Args::pop(const char* const* keys, size_t numberOfKeys) {
  const char* result = nullptr;
  for (size_t i = 0; !result && i < numberOfKeys; i++) {
    result = pop(keys[i]);
  }
  return result;
}

bool Args::popFlag(const char* argument) {
  auto flagIt = find(argument);
  if (flagIt != m_arguments.end()) {
    m_arguments.erase(flagIt);
    return true;
  }
  return false;
}

bool Args::popFlags(const char* const* flags, size_t numberOfFlags) {
  bool result = false;
  for (size_t i = 0; !result && i < numberOfFlags; i++) {
    result = popFlag(flags[i]);
  }
  return result;
}

std::vector<const char*>::const_iterator Args::find(const char* name) const {
  return std::find_if(m_arguments.begin(), m_arguments.end(),
                      [name](const char* s) { return strcmp(s, name) == 0; });
}

#if ION_SIMULATOR_EXTERNAL_APP
static inline int load_eadk_external_data(const char* path) {
  if (path == nullptr) {
    return 0;
  }
  if (eadk_external_data != nullptr || eadk_external_data_size != 0) {
    fprintf(stderr, "Warning: eadk_external_data already loaded\n");
    return -1;
  }
#if __EMSCRIPTEN__
  /* The external data file is downloaded and stored in the file system
   * asynchronously by JS. JS should handle non existing files, we only have to
   * wait for it here. */
  FILE* f;
  while (!(f = fopen(path, "rb"))) {
    Ion::Timing::msleep(100);
  }
#else
  FILE* f = fopen(path, "rb");
  if (f == NULL) {
    fprintf(stderr, "Error loading external data file %s\n", path);
    return -1;
  }
#endif
  fseek(f, 0, SEEK_END);
  eadk_external_data_size = ftell(f);
  fseek(f, 0, SEEK_SET);
  eadk_external_data = static_cast<char*>(malloc(eadk_external_data_size));
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

int main(int argc, char* argv[]) {
  Args args(argc, argv);

#ifndef __WIN32__
#ifdef ASAN
  if (args.popFlag(k_limitStackUsageFlag)) {
    fprintf(stderr, "Unable to have stack limit with ASAN\n");
  }
#else
  /* The goal of this parameter is to limit the stack size on simulator to more
   * closely match the limited stack size of the device, which is 0x8000 bytes
   * (32Ko).
   * To do so, we can call `setrlimit(RLIMIT_STACK, &limit)`. For this new limit
   * to take effect, we need to restart the process with an execv. This is
   * because by the time we make the call to `setrlimit`, the stack is already
   * allocated with a size of 0x7FC000.
   *
   * Unfortunatly, setting a RLIMIT_STACK of 0x8000 produces an EXC_BAD_ACCESS
   * in dyld (iOS Dynamic Link Editor). This is because dyld process exceeds
   * this stack limit. At the time of writing, dyld process takes up at least
   * 0x1C3A0 on the stack, hence reducing the stack size past this limit
   * prohibits the simulator from starting. Because of this, the minimum value
   * possible for the stack limit is 0x1C000+1 which allocate a stack of size
   * 0x20000 (the stack is allocated by increments of 0x4000)
   *
   * Warning: this is pobably dependent of the users architecture.
   * If there is an issue with this parameter going forward, one could try
   * increasing this limit further, maybe dyld takes up more stack space.
   *
   * Debugging tip: While trying to debug with --limit-stack-usage flag on, lldb
   * will automatically stop at the exec call. To bypass this behaviour, write
   * "settings set target.process.stop-on-exec false" in ~/.lldbinit */
  constexpr int k_stackLimit = 0x20000;
  if (args.popFlag(k_limitStackUsageFlag)) {
    struct rlimit stackLimits = {0, 0};
    if (getrlimit(RLIMIT_STACK, &stackLimits) == 0) {
      if (stackLimits.rlim_cur > k_stackLimit) {
        stackLimits = {k_stackLimit, k_stackLimit};
        if (setrlimit(RLIMIT_STACK, &stackLimits) == 0) {
          execv(argv[0], argv);
        }
        fprintf(stderr, "Unable to SET stack limit: ignoring flag %s\n",
                k_limitStackUsageFlag);
      }
    } else {
      fprintf(stderr, "Unable to GET stack limit: ignoring flag %s\n",
              k_limitStackUsageFlag);
    }
  }
#endif
#endif

#if ION_SIMULATOR_FILES
  const char* stateFile =
      args.pop(k_loadStateFileKeys, std::size(k_loadStateFileKeys));
  if (stateFile) {
    assert(Journal::replayJournal());
    bool headlessStateFile = args.popFlag("--headless-state-file");
    StateFile::load(stateFile, headlessStateFile);
    if (args.has(k_languageFlag)) {
      // Override any language setting if there is
      fprintf(stderr,
              "Warning: the language passed as an option will be ignored and "
              "the language of the statefile will be used instead.\n");
      args.pop(k_languageFlag);
    }
    const char* replayJournalLanguage =
        Journal::replayJournal()->startingLanguage();
    if (replayJournalLanguage[0] == 0) {
      /* If the state file contains the wildcard language, still set the
       * language to none so that the initial country is WorldWide and the
       * statefile stays consistent whatever the platform language. */
      replayJournalLanguage = "none";
    }
    args.push(k_languageFlag, replayJournalLanguage);
  }

  const char* screenshotPath = args.pop("--take-screenshot");
  if (screenshotPath) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(screenshotPath);
  }

  const char* allScreenshotsFolder = args.pop("--take-all-screenshots");
  if (allScreenshotsFolder) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(
        allScreenshotsFolder, true);
  }

  bool computeScreenshotsHash = args.popFlag("--compute-hash");
  if (computeScreenshotsHash) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(nullptr, true,
                                                              true);
  }

#if ION_LOG_EVENTS_NAME
  bool doNotLogEvents = args.popFlag("--hide-events");
  if (doNotLogEvents) {
    Ion::Events::SetLogEvents(false);
  }
#endif

#if !defined(_WIN32)
  signal(SIGUSR1, Ion::Simulator::Actions::handleUSR1Sig);
#endif
#endif

  // Default language
  if (!args.has(k_languageFlag)) {
    args.push(k_languageFlag, Platform::languageCode());
  }

  bool headless = args.popFlags(k_headlessFlags, std::size(k_headlessFlags));

  Random::init();
  if (!headless) {
    Journal::init();
    if (args.has(k_languageFlag) && Journal::logJournal()) {
      // Set log journal starting language
      Journal::logJournal()->setStartingLanguage(args.get(k_languageFlag));
    }
    Window::init();
    Haptics::init();
  }

  Ion::Init();
#if ION_SIMULATOR_EXTERNAL_APP
  const char* nwb = args.pop("--nwb");
  if (nwb) {
    if (load_eadk_external_data(args.pop("--nwb-external-data"))) {
      return -1;
    }
    void* handle = dlopen(nwb, RTLD_LAZY);
    if (handle == nullptr) {
      fprintf(stderr, "Error loading %s: %s\n", nwb, dlerror());
      return -1;
    }
    int (*nwb_main)(int argc, const char* const argv[]);
    *(void**)(&nwb_main) = dlsym(handle, "main");
    if (nwb_main == nullptr) {
      /* If the entrypoint is defined in the app as main(int argc, char ** argv)
       * instead of main(), the .wasm will export it with this name (even if the
       * arguments names differ). We handle both symbols for convenience. */
      *(void**)(&nwb_main) = dlsym(handle, "__main_argc_argv");
    }
    if (nwb_main == nullptr) {
      fprintf(stderr, "Could not locate nwb_main symbol: %s\n", dlerror());
      return -1;
    }
    nwb_main(args.argc(), args.argv());
    dlclose(handle);
  } else {
#endif
    ion_main(args.argc(), args.argv());
#if ION_SIMULATOR_EXTERNAL_APP
  }
#endif
  if (!headless) {
    Haptics::shutdown();
    Window::shutdown();
  }

  return 0;
}
