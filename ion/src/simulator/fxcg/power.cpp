#include <ion/power.h>
#include <stdint.h>
#include <string.h>

#include "main.h"

#include <gint/gint.h>
#include <gint/display.h>

void PowerOff(int displayLogo) {
  __asm__ __volatile__ (
    ".align 2 \n\t"
    "mov.l 2f, r2 \n\t"
    "mov.l 1f, r0 \n\t"
    "jmp @r2 \n\t"
    "nop \n\t"
    ".align 2 \n\t"
    "1: \n\t"
    ".long 0x1839 \n\t"
    ".align 4 \n\t"
    "2: \n\t"
    ".long 0x80020070 \n\t"
  );
}

void powerOff(void) {
  PowerOff(1);
}

namespace Ion {
namespace Power {

void suspend(bool checkIfOnOffKeyReleased) {
  Simulator::Main::runPowerOffSafe(powerOff, true);
}

void standby() {
  Simulator::Main::runPowerOffSafe(powerOff, true);
}

}
}
