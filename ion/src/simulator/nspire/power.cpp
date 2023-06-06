#include <ion/power.h>
#include <stdint.h>
#include <string.h>

#include "main.h"

void powerOff(void){
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
