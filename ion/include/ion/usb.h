#ifndef ION_USB_H
#define ION_USB_H

#include <ion/keyboard.h>

namespace Ion {
namespace USB {

bool isPlugged();
bool isEnumerated();  // Speed-enumerated, to be accurate
void clearEnumerationInterrupt();

struct DFUParameters {
  Keyboard::State exitKeys;
  bool bubbleUpEvents;
};

void DFU(DFUParameters parameters = {
             .exitKeys = Keyboard::State(Keyboard::Key::Back),
             .bubbleUpEvents = false});
void enable();
void disable();

}  // namespace USB
}  // namespace Ion

#endif
