#ifndef ION_DEVICE_KERNEL_KEYBOARD_QUEUE_H
#define ION_DEVICE_KERNEL_KEYBOARD_QUEUE_H

#include <ion/keyboard.h>
#include <ion/ring_buffer.h>

namespace Ion {
namespace Device {
namespace Keyboard {

static constexpr size_t k_maximalNumberOfStates = 5;

class Queue : public RingBuffer<Ion::Keyboard::State, k_maximalNumberOfStates> {
public:
  static Queue * sharedQueue();
  void flush(bool resetPending = true);
};

}
}
}

#endif
