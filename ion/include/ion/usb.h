#ifndef ION_USB_H
#define ION_USB_H

namespace Ion {
namespace USB {

bool isPlugged();
bool isEnumerated(); // Speed-enumerated, to be accurate
void clearEnumerationInterrupt();

void DFU(bool exitWithKeyboard = true, bool unlocked = false, int level = 0);
void enable();
void disable();

}
}

#endif
