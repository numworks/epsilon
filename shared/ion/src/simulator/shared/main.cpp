#include <assert.h>
#include <ion.h>
#include <unistd.h>

#if ION_LOG_EVENTS_NAME
#include <ion/keyboard/layout_events.h>
#endif
#include <ion/src/shared/init.h>

#include "arguments.h"
#include "arguments_parser.h"
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

#ifdef __AFL_HAVE_MANUAL_CONTROL
__AFL_FUZZ_INIT();
#endif

int main(int argc, char* argv[]) {
  ArgumentsParser parser(argc, argv);
  Arguments args = parser.parse();

  if (args.help) {
    parser.printHelp();
    return 0;
  }

#if ION_SIMULATOR_FILES
#ifndef __WIN32__
#ifdef ASAN
  if (args.limitStackUsage) {
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
   * Warning: this is probably dependent on the user's architecture.
   * If there is an issue with this parameter going forward, one could try
   * increasing this limit further, maybe dyld takes up more stack space.
   *
   * Debugging tip: While trying to debug with --limit-stack-usage flag on, lldb
   * will automatically stop at the exec call. To bypass this behaviour, write
   * "settings set target.process.stop-on-exec false" in ~/.lldbinit */
  constexpr int k_stackLimit = 0x20000;
  if (args.limitStackUsage) {
    struct rlimit stackLimits = {0, 0};
    if (getrlimit(RLIMIT_STACK, &stackLimits) == 0) {
      if (stackLimits.rlim_cur > k_stackLimit) {
        stackLimits = {k_stackLimit, k_stackLimit};
        if (setrlimit(RLIMIT_STACK, &stackLimits) == 0) {
          execv(argv[0], argv);
        }
        fprintf(stderr, "Unable to SET stack limit: ignoring flag %s\n",
                ArgumentNames::limitStackUsage);
      }
    } else {
      fprintf(stderr, "Unable to GET stack limit: ignoring flag %s\n",
              ArgumentNames::limitStackUsage);
    }
  }
#endif
#endif
#endif

#if ION_SIMULATOR_FILES
  if (args.stateFile) {
    if (strcmp(args.stateFile, "-") != 0 && access(args.stateFile, F_OK) != 0) {
      fprintf(stderr, "Error: state file not found: %s\n", args.stateFile);
      return EXIT_FAILURE;
    }
    assert(Journal::replayJournal());
#ifndef __AFL_HAVE_MANUAL_CONTROL
    StateFile::load(args.stateFile, args.headlessStateFile);
#endif
    if (args.language) {
      // Override any language setting if there is
      fprintf(stderr,
              "Warning: the language passed as an option will be ignored and "
              "the language of the statefile will be used instead.\n");
    }
    const char* replayJournalLanguage =
        Journal::replayJournal()->startingLanguage();
    if (replayJournalLanguage[0] == 0) {
      /* If the state file contains the wildcard language, still set the
       * language to none so that the initial country is WorldWide and the
       * statefile stays consistent whatever the platform language. */
      replayJournalLanguage = "none";
    }
    args.language = replayJournalLanguage;
    args.theme =
        Journal::replayJournal()->startingDarkMode() ? "dark" : "light";
  }

  if (args.screenshotPath) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(
        args.screenshotPath);
  }

  if (args.allScreenshotsFolder) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(
        args.allScreenshotsFolder, true);
  }

  if (args.computeScreenshotsHash) {
    Ion::Simulator::Screenshot::commandlineScreenshot()->init(nullptr, true,
                                                              true);
  }

#if ION_LOG_EVENTS_NAME
  if (args.doNotLogEvents) {
    Ion::Events::SetLogEvents(false);
  }
#endif

#if !defined(_WIN32)
  signal(SIGUSR1, Ion::Simulator::Actions::handleUSR1Sig);
#endif
#endif

  // Default language
  if (!args.language) {
    args.language = Platform::languageCode();
  }

  bool headless = Window::isAlwaysHeadless() || args.headless;

  Random::init(headless);
  if (!headless) {
    Journal::init();
    if (Journal::logJournal()) {
      // Set log journal starting language
      Journal::logJournal()->setStartingLanguage(args.language);
    }
    Window::init();
    Haptics::init();
  }

  Ion::Init();
#if ION_SIMULATOR_EXTERNAL_APP
  if (args.externalApp) {
#ifndef __EMSCRIPTEN__
    if (access(args.externalApp, F_OK) != 0) {
      fprintf(stderr, "Error: external app not found: %s\n", args.externalApp);
      return EXIT_FAILURE;
    }
    if (args.externalAppData && access(args.externalAppData, F_OK) != 0) {
      fprintf(stderr, "Error: external app data not found: %s\n",
              args.externalAppData);
      return EXIT_FAILURE;
    }
#endif
    if (load_eadk_external_data(args.externalAppData)) {
      return EXIT_FAILURE;
    }
    void* handle = dlopen(args.externalApp, RTLD_LAZY);
    if (handle == nullptr) {
      fprintf(stderr, "Error loading %s: %s\n", args.externalApp, dlerror());
      return EXIT_FAILURE;
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
      return EXIT_FAILURE;
    }
    /* This is a workaround for the web platform where we expect the journal to
     * call onEpsilonIdle to remove the loading spinner. We could remove the
     * spinner in onRuntimeInitialized instead but it would cause a small
     * visible delay where the screen is solid gray. */
    Journal::logJournal()->pushEvent(Ion::Events::None);
    nwb_main(parser.argc(), parser.argv());
    dlclose(handle);
  } else {
#endif
    // These arguments are handled by ion_main
    parser.push(ArgumentNames::language, args.language);
    parser.push(ArgumentNames::openApp, args.openApp);
    if (args.theme) {
      parser.push(ArgumentNames::theme, args.theme);
    }
#if __AFL_HAVE_MANUAL_CONTROL
    /* AFL++ deferred forkserver. __AFL_INIT() marks the fork
     * point. Everything above it run once per process and the
     * forkserver snapshots after it. Inputs arrive via shared memory
     * instead of stdin, skipping another process cost.
     * TODO: move it after GlobalBox init (Ion::Init etc)
     */
    __AFL_INIT();
    unsigned char* fuzzBuf = __AFL_FUZZ_TESTCASE_BUF;
    /* TODO: Use a value greater than 1 to enable persistent
     * fuzzing. This requires resetting the state between scenarios
     * which may be hard. */
    while (__AFL_LOOP(1)) {
      int fuzzLen = __AFL_FUZZ_TESTCASE_LEN;
      StateFile::loadMemory(reinterpret_cast<const char*>(fuzzBuf), fuzzLen,
                            args.headlessStateFile);
      ion_main(parser.argc(), parser.argv());
    }
#else
  ion_main(parser.argc(), parser.argv());
#endif
#if ION_SIMULATOR_EXTERNAL_APP
  }
#endif
  if (!headless) {
    Haptics::shutdown();
    Window::shutdown();
  }

  return 0;
}
