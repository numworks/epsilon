#include "device.h"
#include "regs/regs.h"
extern "C" {
#include <assert.h>
}
#include <ion.h>
#include "display.h"


// Public Ion methods

/* TODO: calibrate busy loops */

void Ion::msleep(long ms) {
  for (volatile long i=0; i<8852*ms; i++) {
      __asm volatile("nop");
  }
}
void Ion::usleep(long us) {
  for (volatile long i=0; i<9*us; i++) {
    __asm volatile("nop");
  }
}

// Private Ion::Device methods

namespace Ion {
namespace Device {

void init() {
  Ion::Display::Device::init();
}

}
}
