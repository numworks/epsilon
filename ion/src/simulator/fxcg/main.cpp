#include "main.h"
#include "display.h"
#include "platform.h"

#include <gint/display-cg.h>
#include <gint/gint.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <ion.h>
#include <ion/events.h>

extern "C" {
int main() {
  Ion::Simulator::Main::init();
  ion_main(0, NULL);
  Ion::Simulator::Main::quit();

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

void EnableStatusArea(int opt) {
  __asm__ __volatile__ (
    ".align 2 \n\t"
    "mov.l 2f, r2 \n\t"
    "mov.l 1f, r0 \n\t"
    "jmp @r2 \n\t"
    "nop \n\t"
    ".align 2 \n\t"
    "1: \n\t"
    ".long 0x02B7 \n\t"
    ".align 4 \n\t"
    "2: \n\t"
    ".long 0x80020070 \n\t"
  );
}

extern "C" void *__GetVRAMAddress(void);

uint8_t xyram_backup[16 * 1024];
uint8_t ilram_backup[4 * 1024];

void worldSwitchHandler(void (*worldSwitchFunction)(), bool prepareVRAM) {
  // Back up XYRAM
  uint8_t* xyram = (uint8_t*) 0xe500e000;
  memcpy(xyram_backup, xyram, 16 * 1024);
  // Back up ILRAM
  uint8_t* ilram = (uint8_t*) 0xe5200000;
  memcpy(ilram_backup, ilram, 4 * 1024);

  if (prepareVRAM) {
    // Copying the screen to the OS's VRAM avoids a flicker when powering on
    uint16_t* dst = (uint16_t *) __GetVRAMAddress();
    uint16_t* src = gint_vram + 6;

    for (int y = 0; y < 216; y++, dst += 384, src += 396) {
      for (int x = 0; x < 384; x++) {
        dst[x] = src[x];
      }
    }

    // Disable the status area
    EnableStatusArea(3);
  }

  worldSwitchFunction();

  // Restore XYRAM
  memcpy(xyram, xyram_backup, 16 * 1024);
  // Restore ILRAM
  memcpy(ilram, ilram_backup, 4 * 1024);
}

void runPowerOffSafe(void (*powerOffSafeFunction)(), bool prepareVRAM) {
  gint_world_switch(GINT_CALL(worldSwitchHandler, powerOffSafeFunction, prepareVRAM));
}

}
}
}
