#include "main.h"
#include "display.h"
#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <ion.h>
#include <ion/timing.h>
#include <ion/events.h>

#include <3ds.h>

#include "driver/common.h"

void Ion::Timing::msleep(uint32_t ms) {
    svcSleepThread((s64) ms * 1000);
}

uint64_t Ion::Timing::millis() {
    return svcGetSystemTick() / (1000 * 1000);
}

int main(int argc, char * argv[]) {
  Ion::Simulator::Main::init();

  std::vector<const char *> arguments(argv, argv + argc);

  const char * language = IonSimulatorGetLanguageCode();
  if (language != nullptr) {
    arguments.push_back("--language");
    arguments.push_back(language);
  }

  ion_main(arguments.size(), &arguments[0]);
  Ion::Simulator::Main::quit();

  return 0;
}

namespace Ion {
namespace Simulator {
namespace Main {

static bool sNeedsRefresh = false;

void init() {
  gfxInitDefault();
  cfguInit();
  Ion::Simulator::CommonDriver::init();
  // mcuHwcInit();
  ptmuInit();
  relayout();
}

void relayout() {
  int windowWidth = 800;
  int windowHeight = 240;

  // Keep original aspect ration in screen_only mode.
  /*
  float scale = (float)(Ion::Display::Width) / (float)(Ion::Display::Height);
  if ((float)(windowHeight) * scale > float(windowWidth)) {
    sScreenRect.w = windowWidth;
    sScreenRect.h = (int)((float)(windowWidth) / scale);
  } else {
    sScreenRect.w = (int)((float)(windowHeight) * scale);
    sScreenRect.h = windowHeight;
  }

  sScreenRect.x = (windowWidth - sScreenRect.w) / 2;
  sScreenRect.y = (windowHeight - sScreenRect.h) / 2;
  */

  setNeedsRefresh();
}

void setNeedsRefresh() {
  sNeedsRefresh = true;
}

void refresh() {
  if (!sNeedsRefresh) {
    return;
  }

  Display::draw();
  
  sNeedsRefresh = false;
}

void quit() {
  // mcuHwcExit();
  ptmuExit();
  cfguExit();
  Ion::Simulator::CommonDriver::deinit();
  gfxExit();
}

}
}
}

