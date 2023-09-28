#include <ion/power.h>
#include <stdint.h>
#include <string.h>

#include "main.h"

void powerOff(void){
}


namespace Ion {
namespace Power {

//NOTE: These should probably be reimplemented at some point
//the prior version was a janky mess that flickered a ton on wake
//and was generally awfulness.

void suspend(bool checkIfOnOffKeyReleased) {
 // Simulator::Main::runPowerOffSafe(powerOff, true);
}

void standby() {
 // Simulator::Main::runPowerOffSafe(powerOff, true);
}

}
}
