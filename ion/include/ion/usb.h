#ifndef ION_USB_H
#define ION_USB_H

#include "events.h"

namespace Ion {
namespace USB {

bool isPlugged();
bool isEnumerated(); // Speed-enumerated, to be accurate
void clearEnumerationInterrupt();

Events::Event DFU();
void enable();
void disable();

}
}

#endif
