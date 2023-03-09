#include "helpers.h"

#include <ion.h>

#include "port.h"
extern "C" {
#include "mphalport.h"
}

bool micropython_port_vm_hook_loop() {
  /* This function is called very frequently by the MicroPython engine. We grab
   * this opportunity to interrupt execution and/or refresh the display on
   * platforms that need it. */

  /* Doing too many things here slows down Python execution quite a lot. So we
   * only do things once in a while and return as soon as possible otherwise. */

  static uint64_t t = Ion::Timing::millis();
  constexpr static uint64_t delay = 100;

  uint64_t t2 = Ion::Timing::millis();
  if (t2 - t < delay) {
    return false;
  }
  t = t2;

  micropython_port_vm_hook_refresh_print();
  // Check if the user asked for an interruption from the keyboard
  return micropython_port_interrupt_if_needed();
}

void micropython_port_vm_hook_refresh_print() {
  assert(MicroPython::ExecutionEnvironment::currentExecutionEnvironment() !=
         nullptr);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()
      ->refreshPrintOutput();
}

bool micropython_port_interruptible_msleep(int32_t delay) {
  static uint64_t lastRun = 0;
  assert(delay >= 0);
  constexpr int32_t miniumDelayBetweenInterruptions = 25;
  constexpr int32_t interruptionCheckDelay = 100;
  const int32_t numberOfInterruptionChecks = delay / interruptionCheckDelay;
  int32_t remainingDelay =
      delay - numberOfInterruptionChecks * interruptionCheckDelay;
  int32_t currentRemainingInterruptionChecks = numberOfInterruptionChecks;
  do {
    int32_t timeToSleep =
        currentRemainingInterruptionChecks == numberOfInterruptionChecks
            ? remainingDelay
            : interruptionCheckDelay;
    /* On simulators the interruption check also refreshes the screen and takes
     * about 15ms. We make sure not to execute it too often to ensure a
     * consistent speed between platforms. */
    uint64_t run = Ion::Timing::millis();
    if (run - lastRun > miniumDelayBetweenInterruptions) {
      if (micropython_port_interrupt_if_needed()) {
        return true;
      }
      lastRun = run;
      /* Remove from timeToSleep the time taken by the check */
      timeToSleep -= Ion::Timing::millis() - run;
    }
    if (timeToSleep > 0) {
      Ion::Timing::msleep(timeToSleep);
    }
    currentRemainingInterruptionChecks--;
  } while (currentRemainingInterruptionChecks >= 0);
  return false;
}

bool micropython_port_interrupt_if_needed() {
  Ion::Keyboard::State state(0);
  Ion::Keyboard::Key interruptKey =
      static_cast<Ion::Keyboard::Key>(mp_interrupt_char);
  bool interruption = false;
  while (state != Ion::Keyboard::State(-1)) {
    interruption = state.keyDown(interruptKey) ? true : interruption;
    state = Ion::Keyboard::scanForInterruptionAndPopState();
  };

  if (interruption) {
    mp_keyboard_interrupt();
    return true;
  }
  return false;
}

int micropython_port_random() { return Ion::random(); }
