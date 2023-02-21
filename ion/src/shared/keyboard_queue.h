#ifndef ION_SHARED_KEYBOARD_QUEUE_H
#define ION_SHARED_KEYBOARD_QUEUE_H

#include <ion/keyboard.h>
#include <ion/ring_buffer.h>

namespace Ion {
namespace Keyboard {

constexpr static size_t k_maximalNumberOfStates = 5;

class Queue {
 public:
  static Queue* sharedQueue();

  void flush(bool resetPreemptiveState = true);
  void push(State s);
  State queuePop();
  size_t length() const { return m_buffer.length(); }
  bool isEmpty() const { return m_buffer.isEmpty(); }

 private:
  Queue() : m_pushedWhileBusy(-1), m_busy(false) {}

  void reset();
  void didFlush(bool resetPending);
  void handleBusyState();

  RingBuffer<State, k_maximalNumberOfStates> m_buffer;
  /* The push method is called from a high-priority interruption, and can as
   * such be executed in the middle of a pop or reset. Conversely, pop and
   * reset are called from low-priority interrupts (svcall and pendsv
   * respectively), and cannot interrupt other queue operations (svcall could
   * theoretically interrupt pendsv as its priority is higher but this cannot
   * happen in practice as the pendsv handler does not use SVCs).
   * FIXME If a push occurs while the queue is busy, the State is temporarily
   * stored in m_pushedWhileBusy, to be pushed later when the original
   * operation completes. A more thorough solution would be to temporarily
   * increase the current interruption priority (active interruptions can be
   * infered from the NVIC_IABRx register). If other problems involving
   * concurrency arise, we could develop a Transaction class based on this
   * mechanism. */
  State m_pushedWhileBusy;
  bool m_busy;
};

}  // namespace Keyboard
}  // namespace Ion

#endif
