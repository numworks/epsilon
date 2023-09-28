#include "main.h"
#include "display.h"
#include "platform.h"


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <ion.h>
#include <ion/events.h>
#include "k_csdk.h"

static const char * storage_name="nwstore.nws";

int save_state(const char * fname); // apps/home/controller.cpp

extern "C" {
  extern const int prizm_heap_size;
  const int prizm_heap_size=1024*1024;
  __attribute__((aligned(4))) char prizm_heap[prizm_heap_size];

  int calculator=4; // -1 means OS not checked, 0 unknown, 1 cg50 or 90, 2 emu 50 or 90, 3 other

  int main() {
    sdk_init(); //this calls the lcd init functions from behind the scenes
    Ion::Simulator::Main::init();
    ion_main(0, NULL);
    Ion::Simulator::Main::quit();
    sdk_end();
    return 0;
  }
}

namespace Ion {
namespace Simulator {
namespace Main {

static bool sNeedsRefresh = false;

void init() {
  Ion::Simulator::Display::init();
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
  Ion::Simulator::Display::quit();
}

}
}
}
