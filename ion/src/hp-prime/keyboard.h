#ifndef ION_HP_PRIME_KEYBOARD_H
#define ION_HP_PRIME_KEYBOARD_H

#include <ion/keyboard.h>
#include <ion.h>
#include "regs/regs.h"

namespace Ion {
namespace Keyboard {
namespace Device {

void init();
uint64_t scan();

}
}
}

#endif
