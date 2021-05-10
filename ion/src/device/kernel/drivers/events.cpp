#include <kernel/drivers/events.h>
#include <algorithm>
#include <assert.h>
#include <drivers/battery.h>
#include <drivers/config/clocks.h>
#include <drivers/display.h>
#include <drivers/usb_privileged.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/circuit_breaker.h>
#include <kernel/drivers/config/keyboard.h>
#include <kernel/drivers/keyboard_queue.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <ion/keyboard.h>
#include <ion/usb.h>
#include <ion/src/shared/events_modifier.h>
#include <layout_events.h>
#include <limits.h>
#include <string.h>

namespace Ion {
namespace Events {

const char * Event::text() const {
  return defaultText();
}

}
}

namespace Ion {
namespace Device {
namespace Events {

using namespace Regs;

size_t copyTextSecure(uint8_t eventId, char * buffer, size_t bufferSize) {
  Ion::Events::Event e(eventId);
  if (e.text()) {
    return strlcpy(buffer, e.text(), bufferSize);
  } else {
    return 0;
  }
}

bool isDefinedSecure(uint8_t eventId) {
  Ion::Events::Event e(eventId);
  if (e.isKeyboardEvent()) {
    return Ion::Events::s_dataForEvent[static_cast<uint8_t>(e)].isDefined();
  } else {
    return (e == Ion::Events::None || e == Ion::Events::Termination || e == Ion::Events::USBEnumeration || e == Ion::Events::USBPlug || e == Ion::Events::BatteryCharging || e == Ion::Events::ExternalText);
  }
}

/* We want to prescale the timer to be able to set the auto-reload in
 * milliseconds. However, since the prescaler range is 2^16-1, we use a factor
 * not to overflow PSC. */
static constexpr int k_prescalerFactor = 4;
static constexpr int k_stallingDelay = 2*1000*k_prescalerFactor; // TODO: calibrate

void initTimer() {
  TIM2.PSC()->set(Clocks::Config::APB1TimerFrequency*1000/k_prescalerFactor-1);
  TIM2.DIER()->setUIE(true);
  TIM2.ARR()->set(k_stallingDelay);
}

void shutdownTimer() {
  TIM2.DIER()->setUIE(false);
  TIM2.CR1()->setCEN(false);
}

static constexpr int tim2interruptionISRIndex = 28;

void initInterruptions() {
  // Flush pending interruptions
  while (NVIC.NVIC_ICPR()->getBit(tim2interruptionISRIndex)) { // Read to force writing
    NVIC.NVIC_ICPR()->setBit(tim2interruptionISRIndex, true);
  }
  /* Configure the priority: the event stalling interruption should not
   * interrupt SVCalls (send data to display...) */
  NVIC.NVIC_IPR()->setPriority(tim2interruptionISRIndex, NVIC::NVIC_IPR::InterruptionPriority::MediumLow);
  // Enable interruptions
  NVIC.NVIC_ISER()->setBit(tim2interruptionISRIndex, true);

  initTimer();
}

void shutdownInterruptions() {
  shutdownTimer();
  // Disable interruptions
  NVIC.NVIC_ICER()->setBit(tim2interruptionISRIndex, true);
}

void init() {
  initInterruptions();
}

void shutdown() {
  shutdownInterruptions();
}

bool sLastUSBPlugged = false;
bool sLastUSBEnumerated = false;
bool sLastBatteryCharging = false;

Ion::Events::Event getPlatformEvent() {
  // First, check if the USB device has been connected to an USB host
  bool usbEnumerated = Ion::USB::isEnumerated();
  if (usbEnumerated != sLastUSBEnumerated) {
    sLastUSBPlugged = Ion::USB::isPlugged();
    sLastBatteryCharging = Battery::isCharging();
    sLastUSBEnumerated = usbEnumerated;
    if (usbEnumerated) {
      return Ion::Events::USBEnumeration;
    }
  }

  // Second, check if the USB plugged status has changed
  bool usbPlugged = Ion::USB::isPlugged();
  if (usbPlugged != sLastUSBPlugged) {
    sLastUSBPlugged = usbPlugged;
    sLastBatteryCharging = Battery::isCharging();
    return Ion::Events::USBPlug;
  }

  // Third, check if the battery changed charging state
  bool batteryCharging = Battery::isCharging();
  if (batteryCharging != sLastBatteryCharging) {
    sLastBatteryCharging = batteryCharging;
    return Ion::Events::BatteryCharging;
  }
  return Ion::Events::None;
}

constexpr int delayBeforeRepeat = 200;
constexpr int delayBetweenRepeat = 50;
Ion::Events::Event sLastEvent = Ion::Events::None;
Ion::Keyboard::State sCurrentKeyboardState(0);
Ion::Keyboard::State sLastKeyboardState(0);
bool sLastEventShift;
bool sLastEventAlpha;
bool sEventIsRepeating = false;
Ion::Keyboard::State sPreemtiveState(0);

bool canRepeatEventWithState() {
  return canRepeatEvent(sLastEvent)
    && sCurrentKeyboardState == sLastKeyboardState
    && sLastEventShift == sCurrentKeyboardState.keyDown(Ion::Keyboard::Key::Shift)
    && sLastEventAlpha == (sCurrentKeyboardState.keyDown(Ion::Keyboard::Key::Alpha) || Ion::Events::isLockActive());
}

bool handlePreemption(bool stalling) {
  Ion::Keyboard::State currentPreemptiveState = sPreemtiveState;
  sPreemtiveState = Ion::Keyboard::State(0);
  if (currentPreemptiveState.keyDown(Ion::Keyboard::Key::Home)) {
    if (CircuitBreaker::hasCheckpoint(Ion::CircuitBreaker::CheckpointType::Home)) {
      CircuitBreaker::loadCheckpoint(Ion::CircuitBreaker::CheckpointType::Home);
      return true;
    }
    return false;
  }
  if (currentPreemptiveState.keyDown(Ion::Keyboard::Key::OnOff)) {
    if (stalling && CircuitBreaker::hasCheckpoint(Ion::CircuitBreaker::CheckpointType::User)) {
      /* If we are still processing an event (stalling) and in a middle of a
       * "hard-might-be-long computation" (custom checkpoint is set), we
       * checkout the Custom checkpoint and try again to wait for the event to
       * be processed in case we can avoid preemptively switching off. */
      sPreemtiveState = currentPreemptiveState;
      CircuitBreaker::loadCheckpoint(Ion::CircuitBreaker::CheckpointType::User);
      return true;
    }
    Power::suspend(true);
    if (stalling && CircuitBreaker::hasCheckpoint(Ion::CircuitBreaker::CheckpointType::Home)) {
      /* If we were stalling (in the middle of processing an event), we load
       * the Home checkpoint to avoid resuming the execution in the middle of
       * redrawing the display for instance. */
      CircuitBreaker::loadCheckpoint(Ion::CircuitBreaker::CheckpointType::Home);
      return true;
    }
    return false;
  }
  if (currentPreemptiveState.keyDown(Ion::Keyboard::Key::Back)) {
    if (stalling && CircuitBreaker::hasCheckpoint(Ion::CircuitBreaker::CheckpointType::User)) {
      Keyboard::Queue::sharedQueue()->flush();
      CircuitBreaker::loadCheckpoint(Ion::CircuitBreaker::CheckpointType::User);
      return true;
    } else {
      Keyboard::Queue::sharedQueue()->push(currentPreemptiveState);
      return false;
    }
  }
  assert(currentPreemptiveState == Ion::Keyboard::State(0));
  return false;
}

Ion::Events::Event nextEvent(int * timeout) {
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);

  // Handle preemptive event before time is out
  if (handlePreemption(false)) {
    /* If handlePreemption returns true, it means a PendSV was generated. We
     * return early to speed up the context switch (otherwise, PendSV will wait
     * until the SVCall ends). */
    return Ion::Events::None;
  }

  uint64_t keysSeenUp = -1;
  uint64_t keysSeenTransitionningFromUpToDown = 0;
  uint64_t startTime = Ion::Timing::millis();
  while (true) {
    /* NB: Currently, platform events are polled. They could be instead linked
     * to EXTI interruptions and their event could be pushed on the
     * Keyboard::Queue. However, the pins associated with platform events are
     * the following:
     * +----------------------+------------+-------+-------+
     * |   PlatformEvent      |   Pin name | N0100 | N0110 |
     * +----------------------+------------+-------+-------+
     * | Battery::isCharging  | CharingPin |   A0  |   E3  |
     * |  USB::isPlugged      | VBus       |   A9  |   A9  |
     * +----------------------+------------+-------+-------+
     *
     * The EXTI lines 0 and 3 are already used by the keyboard interruptions.
     * We could linked an interruption to USB::isPlugged and to
     * USB::isEnumerated (through EXTI 18 - USB On-The-Go FS Wakeup) but we
     * could not get an interruption on the end of charge. For more consistency,
     * the three platform events are retrieved through polling.
     */
    Ion::Events::Event platformEvent = getPlatformEvent();
    if (platformEvent != Ion::Events::None) {
      return platformEvent;
    }

    while (!Keyboard::Queue::sharedQueue()->isEmpty()) {
      sCurrentKeyboardState = Keyboard::Queue::sharedQueue()->pop();

      keysSeenUp |= ~sCurrentKeyboardState;
      keysSeenTransitionningFromUpToDown = keysSeenUp & sCurrentKeyboardState;

      if (keysSeenTransitionningFromUpToDown != 0) {
        sEventIsRepeating = false;
        Ion::Events::resetLongRepetition();
        /* The key that triggered the event corresponds to the first non-zero bit
         * in "match". This is a rather simple logic operation for the which many
         * processors have an instruction (ARM thumb uses CLZ).
         * Unfortunately there's no way to express this in standard C, so we have
         * to resort to using a builtin function. */
        Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(63-__builtin_clzll(keysSeenTransitionningFromUpToDown));
        bool shift = Ion::Events::isShiftActive() || sCurrentKeyboardState.keyDown(Ion::Keyboard::Key::Shift);
        bool alpha = Ion::Events::isAlphaActive() || sCurrentKeyboardState.keyDown(Ion::Keyboard::Key::Alpha);
        Ion::Events::Event event(key, shift, alpha, Ion::Events::isLockActive());
        sLastEventShift = shift;
        sLastEventAlpha = alpha;
        updateModifiersFromEvent(event);
        sLastEvent = event;
        sLastKeyboardState = sCurrentKeyboardState;
        return event;
      }
    }

    int delay = *timeout;
    int delayForRepeat = INT_MAX;
    bool potentialRepeatingEvent = canRepeatEventWithState();
    if (potentialRepeatingEvent) {
      delayForRepeat = (sEventIsRepeating ? delayBetweenRepeat : delayBeforeRepeat);
      delay = std::min(delay, delayForRepeat);
    }

    int elapsedTime = 0;
    bool keyboardInterruptionOccured = false;
    Board::setClockLowFrequency();
    while (elapsedTime < delay) {
      // Stop until either systick or a keyboard/platform interruption happens
      /* TODO: - optimization - we could maybe shutdown systick interrution and
       * set a longer interrupt timer which would udpate systick millis and
       optimize the interval of time the execution is stopped. */
      asm("wfi");
      if (!Keyboard::Queue::sharedQueue()->isEmpty()) {
        keyboardInterruptionOccured = true;
        break;
      }
      elapsedTime = static_cast<int>(Ion::Timing::millis() - startTime);
    }
    Board::setClockStandardFrequency();
    *timeout = std::max(0, *timeout - elapsedTime);
    startTime = Ion::Timing::millis();

    // If the wake up was due to a keyboard/platformEvent
    if (keyboardInterruptionOccured) {
      continue;
    }

    // Timeout
    if (*timeout == 0) {
      Ion::Events::resetLongRepetition();
      return Ion::Events::None;
    }

    /* At this point, we know that keysSeenTransitionningFromUpToDown has
     * always been zero. In other words, no new key has been pressed. */
    if (elapsedTime >= delayForRepeat) {
      assert(potentialRepeatingEvent);
      sEventIsRepeating = true;
      Ion::Events::incrementRepetitionFactor();
      return sLastEvent;
    }
  }
}

void resetStallingTimer() {
  // Init timer on the first call to getEvent
  TIM2.CR1()->setCEN(true);
  // Reset the counter
  TIM2.CNT()->set(0);
}

Ion::Events::Event getEvent(int * timeout) {
  Ion::Events::Event e = nextEvent(timeout);
  resetStallingTimer();
  return e;
}

static bool s_spinner = true;

void setSpinner(bool spinner) {
  s_spinner = spinner;
}

void stall() {
  // Clear update interrupt flag
  TIM2.SR()->setUIF(false);

  if (handlePreemption(true)) {
    return;
  }

  if (s_spinner) {
  /* TODO: set another quick timer that would restore the image below in a few ms...*/
  //if (CircuitBreaker::hasCheckpoint()) {
    // TODO: should we shutdown any interruption here to be sure to complete our drawing
    // TODO: draw a hourglass or spinning circle?
    static KDColor c = KDColorGreen;
    Ion::Device::Display::pushRectUniform(KDRect(155,115,10,10), c);
    c = c == KDColorGreen ? KDColorRed : KDColorGreen;

  /*} else {
    // TODO: go back to the home!
  }*/
  }
}

void resetPendingKeyboardState() {
  sPreemtiveState = Ion::Keyboard::State(0);
}

bool setPendingKeyboardStateIfPreemtive(Ion::Keyboard::State state) {
  if (state.keyDown(Ion::Keyboard::Key::Home) || state.keyDown(Ion::Keyboard::Key::Back) || state.keyDown(Ion::Keyboard::Key::OnOff)) {
    sPreemtiveState = state;
    return true;
  }
  return false;
}

}
}
}
