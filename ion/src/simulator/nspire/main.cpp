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
    sdk_init();
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

void runPowerOffSafe(void (*powerOffSafeFunction)(), bool prepareVRAM) {
  // somewhat OFF by setting LCD to 0
  unsigned NSPIRE_CONTRAST_ADDR=is_cx2?0x90130014:0x900f0020;
  unsigned oldval=*(volatile unsigned *)NSPIRE_CONTRAST_ADDR,oldval2;
  if (is_cx2){
    oldval2=*(volatile unsigned *) (NSPIRE_CONTRAST_ADDR+4);
    *(volatile unsigned *) (NSPIRE_CONTRAST_ADDR+4)=0xffff;
  }
  *(volatile unsigned *)NSPIRE_CONTRAST_ADDR=is_cx2?0xffff:0x100;
  static volatile uint32_t *lcd_controller = (volatile uint32_t*) 0xC0000000;
  lcd_controller[6] &= ~(0b1 << 11);
  loopsleep(20);
  lcd_controller[6] &= ~ 0b1; 
  unsigned NSPIRE_RTC_ADDR=0x90090000;
  unsigned offtime=* (volatile unsigned *) NSPIRE_RTC_ADDR;
  for (int n=0;!on_key_pressed();++n){
    loopsleep(100);
    idle();
  }
  lcd_controller[6] |= 0b1;
  loopsleep(20);
  lcd_controller[6]|= 0b1 << 11;
  if (is_cx2)
    *(volatile unsigned *)(NSPIRE_CONTRAST_ADDR+4)=oldval2;
  *(volatile unsigned *)NSPIRE_CONTRAST_ADDR=oldval;
  sync_screen();

}

}
}
}
