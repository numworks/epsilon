#include "haptics.h"
#include "platform.h"
#include "telemetry.h"
#include "window.h"
#include <vector>
#include <ion.h>

int main(int argc, char * argv[]) {
  std::vector<const char *> arguments(argv, argv + argc);

  char * language = IonSimulatorGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

#if EPSILON_TELEMETRY
  Ion::Simulator::Telemetry::init();
#endif
  Ion::Simulator::Window::init();
  Ion::Simulator::Haptics::init();
  ion_main(arguments.size(), &arguments[0]);
  Ion::Simulator::Haptics::shutdown();
  Ion::Simulator::Window::quit();
#if EPSILON_TELEMETRY
  Ion::Simulator::Telemetry::shutdown();
#endif

  return 0;
}
