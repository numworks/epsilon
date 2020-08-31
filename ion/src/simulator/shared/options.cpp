#include <argh.h>
#ifndef __WIN32__
#include <signal.h>
#include <sys/resource.h>
#endif
#include "framebuffer.h"
#include "platform.h"
#include "apps/apps_container.h"
#include "apps/code/app.h"

namespace Ion {
namespace Simulator {
namespace Options {

static bool sHeadless = false;

int parse(int argc, char * argv[]) {
  argh::parser arguments(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

#ifndef __WIN32__
  // Limit stack size
  if (arguments["limit-stack"]) {
    constexpr int kStackSize = 32768;
    struct rlimit stackLimits = {kStackSize, kStackSize};
    setrlimit(RLIMIT_STACK, &stackLimits);
  }
#endif

  // Headless
  if (arguments["headless"]) {
    sHeadless = true;
    Ion::Simulator::Framebuffer::setActive(false);
  }

  // Language
  std::string language = arguments("language").str();
  if (language.empty()) {
    language = std::string(IonSimulatorGetLanguageCode());
  }
  for (int i = 0; i < I18n::NumberOfLanguages; i++) {
    if (language == I18n::LanguageISO6391Codes[i]) {
      GlobalPreferences::sharedGlobalPreferences()->setLanguage((I18n::Language)i);
      break;
    }
  }

  // Python script
  std::string scriptArgument = arguments("code-script").str();
  if (!scriptArgument.empty()) {
    size_t colonIndex = scriptArgument.find(":");
    if (colonIndex != std::string::npos) {
      std::string scriptName = scriptArgument.substr(0, colonIndex);
      std::string scriptContent = scriptArgument.substr(colonIndex+1);
      App::Snapshot * snapshot = AppsContainer::sharedAppsContainer()->snapshotNamed("code");
      Code::App::Snapshot * code = reinterpret_cast<Code::App::Snapshot *>(snapshot);
      if (code) {
        code->loadScript(scriptName.c_str(), scriptContent.c_str());
      }
    }
  }

  return 0;
}

bool headless() {
  return sHeadless;
}

}
}
}
