#include <drivers/battery.h>
#include <drivers/config/clocks.h>
#include <drivers/config/keyboard.h>
#include <drivers/display.h>
#include <drivers/usb_privileged.h>
#include <ion/keyboard.h>
#include <ion/src/shared/keyboard_queue.h>
#include <ion/src/shared/events.h>
#include <ion/src/shared/events_modifier.h>
#include <ion/usb.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/circuit_breaker.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/power.h>
#include <kernel/drivers/timing.h>
#include <algorithm>
#include <assert.h>
#include <layout_events.h>
#include <limits.h>

namespace Ion {
namespace Events {

// ion/src/shared/events.h

Keyboard::State sPreemtiveState(0);
bool sStalling = false;

bool handlePreemption(bool stalling) {
  Ion::Keyboard::State currentPreemptiveState = sPreemtiveState;
  sPreemtiveState = Ion::Keyboard::State(0);

  if (currentPreemptiveState.keyDown(Keyboard::Key::Home)) {
    if (Device::CircuitBreaker::hasCheckpoint(CircuitBreaker::CheckpointType::Home)) {
      Device::CircuitBreaker::loadCheckpoint(CircuitBreaker::CheckpointType::Home);
      return true;
    }
  } else if (currentPreemptiveState.keyDown(Keyboard::Key::OnOff)) {
    Device::Power::suspend(true);
    if (stalling && Device::CircuitBreaker::hasCheckpoint(CircuitBreaker::CheckpointType::Home)) {
      /* If we were stalling (in the middle of processing an event), we load
       * the Home checkpoint to avoid resuming the execution in the middle of
       * redrawing the display for instance. */
      Device::CircuitBreaker::loadCheckpoint(CircuitBreaker::CheckpointType::Home);
      return true;
    }
  } else if (currentPreemptiveState.keyDown(Keyboard::Key::Back)) {
    if (stalling && Device::CircuitBreaker::hasCheckpoint(CircuitBreaker::CheckpointType::User)) {
      Device::CircuitBreaker::loadCheckpoint(CircuitBreaker::CheckpointType::User);
      return true;
    }
  } else {
    assert(currentPreemptiveState == Ion::Keyboard::State(0));
  }
  return false;
}

bool sLastUSBPlugged = false;
bool sLastUSBEnumerated = false;
bool sLastBatteryCharging = false;

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
Ion::Events::Event getPlatformEvent() {
  // First, check if the USB device has been connected to an USB host
  bool usbEnumerated = Ion::USB::isEnumerated();
  if (usbEnumerated != sLastUSBEnumerated) {
    sLastUSBEnumerated = usbEnumerated;
    if (usbEnumerated) {
      sLastUSBPlugged = Ion::USB::isPlugged();
      sLastBatteryCharging = Device::Battery::isCharging();
      return Ion::Events::USBEnumeration;
    }
  }

  // Second, check if the USB plugged status has changed
  bool usbPlugged = Ion::USB::isPlugged();
  if (usbPlugged != sLastUSBPlugged) {
    sLastUSBPlugged = usbPlugged;
    sLastBatteryCharging = Device::Battery::isCharging();
    return Ion::Events::USBPlug;
  }

  // Third, check if the battery changed charging state
  bool batteryCharging = Device::Battery::isCharging();
  if (batteryCharging != sLastBatteryCharging) {
    sLastBatteryCharging = batteryCharging;
    return Ion::Events::BatteryCharging;
  }
  return Ion::Events::None;
}

void didPressNewKey() {}

bool waitForInterruptingEvent(int maximumDelay, int * timeout) {
  int startTime = Timing::millis();
  int elapsedTime = 0;
  bool eventOccurred = false;
  Device::Board::setClockLowFrequency();
  while (elapsedTime < maximumDelay) {
    // Stop until either systick or a keyboard/platform interruption happens
    /* TODO: - optimization - we could maybe shutdown systick interrution and
     * set a longer interrupt timer which would udpate systick millis and
     * optimize the interval of time the execution is stopped. */
    asm("wfi");
    elapsedTime = static_cast<int>(Ion::Timing::millis() - startTime);
    if (!Device::Keyboard::Queue::sharedQueue()->isEmpty()) {
      eventOccurred = true;
      break;
    }
  }
  Device::Board::setClockStandardFrequency();
  *timeout = std::max(0, *timeout - elapsedTime);
  return eventOccurred;
}

void resetPendingKeyboardState() {
  sPreemtiveState = Ion::Keyboard::State(0);
}

void setPendingKeyboardStateIfPreemtive(Keyboard::State state) {
  if (state.keyDown(Ion::Keyboard::Key::Home) || state.keyDown(Ion::Keyboard::Key::Back) || state.keyDown(Ion::Keyboard::Key::OnOff)) {
    sPreemtiveState = state;
  }
}

// ion/include/ion/events.h

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
  return Ion::Events::sharedCopyText(eventId, buffer, bufferSize);
}

bool isDefinedSecure(uint8_t eventId) {
  return Ion::Events::sharedIsDefined(eventId);
}

/* We want to prescale the timer to be able to set the auto-reload in
 * milliseconds. However, since the prescaler range is 2^16-1, we use a factor
 * not to overflow PSC. */
static constexpr int k_prescalerFactor = 4;
static constexpr int k_stallingDelay = 500*k_prescalerFactor; // TODO: calibrate
static constexpr int k_spinningDelay = 100*k_prescalerFactor; // TODO: calibrate

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

  // Init timer
  TIM2.init(Clocks::Config::APB1TimerFrequency*1000/k_prescalerFactor-1, k_stallingDelay);
}

void shutdownInterruptions() {
  // Shutdown timer
  TIM2.shutdown();

  // Disable interruptions
  NVIC.NVIC_ICER()->setBit(tim2interruptionISRIndex, true);
}

void init() {
  initInterruptions();
}

void shutdown() {
  shutdownInterruptions();
}

Ion::Events::Event getEvent(int * timeout) {
  Ion::Events::Event e = Ion::Events::sharedGetEvent(timeout);
  resetStallingTimer();
  return e;
}

static KDColor s_spinnerFullPixels[k_spinnerHalfHeight][k_spinnerHalfWidth] = {
  { KDColor::RGB24(0xEDB558), KDColor::RGB24(0xF7CE5C), KDColor::RGB24(0xF5DE7E), KDColor::RGB24(0xFDF6C2), KDColor::RGB24(0xF6F3BE), KDColor::RGB24(0xE9D767), KDColor::RGB24(0xFBC42C), KDColor::RGB24(0xFFAA15) },
  { KDColor::RGB24(0xE8CE45), KDColor::RGB24(0xFFE6B9), KDColor::RGB24(0xFFE8ED), KDColor::RGB24(0xFFEDDC), KDColor::RGB24(0xFCECDC), KDColor::RGB24(0xFFE4E4), KDColor::RGB24(0xFFDB98), KDColor::RGB24(0xF1BE0D) },
  { KDColor::RGB24(0xEDE894), KDColor::RGB24(0xFFEEF1), KDColor::RGB24(0xFFDBDC), KDColor::RGB24(0xF2D380), KDColor::RGB24(0xEFD98D), KDColor::RGB24(0xFFE4F0), KDColor::RGB24(0xFFEEF2), KDColor::RGB24(0xEEDB7C) },
  { KDColor::RGB24(0xF8F9F4), KDColor::RGB24(0xFAE4BF), KDColor::RGB24(0xE9BF53), KDColor::RGB24(0xEBBD12), KDColor::RGB24(0xEDC92A), KDColor::RGB24(0xEED885), KDColor::RGB24(0xFCF3E4), KDColor::RGB24(0xFAF7FF) }
};

static KDColor s_spinnerFirstStepPixels[k_spinnerHalfHeight][k_spinnerHalfWidth] = {
  { KDColor::RGB24(0xFFB734), KDColor::RGB24(0xFFB731), KDColor::RGB24(0xFFDA95), KDColor::RGB24(0xFEE5BD), KDColor::RGB24(0xFED995), KDColor::RGB24(0xFFBF4E), KDColor::RGB24(0xFFB636), KDColor::RGB24(0xFCB934) },
  { KDColor::RGB24(0xFCB535), KDColor::RGB24(0xFFBB34), KDColor::RGB24(0xFCFEF9), KDColor::RGB24(0xFBFFFE), KDColor::RGB24(0xFFFFFD), KDColor::RGB24(0xFFFBF2), KDColor::RGB24(0xFFC55D), KDColor::RGB24(0xFFB734) },
  { KDColor::RGB24(0xFEB834), KDColor::RGB24(0xFFB532), KDColor::RGB24(0xFFD796), KDColor::RGB24(0xFCC65A), KDColor::RGB24(0xFED07A), KDColor::RGB24(0xFFF9EF), KDColor::RGB24(0xFFFCED), KDColor::RGB24(0xFFCE79) },
  { KDColor::RGB24(0xFFB732), KDColor::RGB24(0xFCB936), KDColor::RGB24(0xFFB435), KDColor::RGB24(0xFFB933), KDColor::RGB24(0xFFB636), KDColor::RGB24(0xFFC45C), KDColor::RGB24(0xFFFEFF), KDColor::RGB24(0xFFFFFD) }
};

static KDColor s_spinnerSecondStepPixels[k_spinnerHalfHeight][k_spinnerHalfWidth] = {
  { KDColor::RGB24(0xFFB631), KDColor::RGB24(0xFEBC44), KDColor::RGB24(0xFFB633), KDColor::RGB24(0xFFB636), KDColor::RGB24(0xFFDA93), KDColor::RGB24(0xFEC14C), KDColor::RGB24(0xFFB633), KDColor::RGB24(0xFFB633) },
  { KDColor::RGB24(0xFCBC40), KDColor::RGB24(0xFFFAEE), KDColor::RGB24(0xFEB834), KDColor::RGB24(0xFDB932), KDColor::RGB24(0xFFFFFF), KDColor::RGB24(0xFFFBF2), KDColor::RGB24(0xFFC65D), KDColor::RGB24(0xFDB735) },
  { KDColor::RGB24(0xFFE1AD), KDColor::RGB24(0xFFFEFD), KDColor::RGB24(0xFEB832), KDColor::RGB24(0xFDB735), KDColor::RGB24(0xFED078), KDColor::RGB24(0xFFF9F4), KDColor::RGB24(0xFFFBF0), KDColor::RGB24(0xFED07B) },
  { KDColor::RGB24(0xFFF3E3), KDColor::RGB24(0xFDE4AC), KDColor::RGB24(0xFFB635), KDColor::RGB24(0xFDB836), KDColor::RGB24(0xFFB631), KDColor::RGB24(0xFBC559), KDColor::RGB24(0xFEFEFF), KDColor::RGB24(0xFEFFFA) }
};

static KDColor s_spinnerThirdStepPixels[k_spinnerHalfHeight][k_spinnerHalfWidth] = {
  { KDColor::RGB24(0xFEB834), KDColor::RGB24(0xFFBB42), KDColor::RGB24(0xFFDA93), KDColor::RGB24(0xFCE5BC), KDColor::RGB24(0xFEB832), KDColor::RGB24(0xFFB538), KDColor::RGB24(0xFFB736), KDColor::RGB24(0xFFB633) },
  { KDColor::RGB24(0xFFBC42), KDColor::RGB24(0xFEFCEF), KDColor::RGB24(0xFEFEFE), KDColor::RGB24(0xFFFFFD), KDColor::RGB24(0xFFB731), KDColor::RGB24(0xFFB734), KDColor::RGB24(0xFFC662), KDColor::RGB24(0xFDB932) },
  { KDColor::RGB24(0xFEE2B0), KDColor::RGB24(0xFAFFFF), KDColor::RGB24(0xFFD996), KDColor::RGB24(0xFFC45F), KDColor::RGB24(0xFFB734), KDColor::RGB24(0xFEB836), KDColor::RGB24(0xFEFAEF), KDColor::RGB24(0xFFCE79) },
  { KDColor::RGB24(0xFEF5E4), KDColor::RGB24(0xFFE2AF), KDColor::RGB24(0xFDB733), KDColor::RGB24(0xFCB535), KDColor::RGB24(0xFEB834), KDColor::RGB24(0xFFB734), KDColor::RGB24(0xFFFEFF), KDColor::RGB24(0xFFFFFD) }
};

static KDColor s_spinnerFourthStepPixels[k_spinnerHalfHeight][k_spinnerHalfWidth] = {
  { KDColor::RGB24(0xFFB734), KDColor::RGB24(0xFFBB42), KDColor::RGB24(0xFCD995), KDColor::RGB24(0xFFE7B9), KDColor::RGB24(0xFDDB91), KDColor::RGB24(0xFFC24F), KDColor::RGB24(0xFFB538), KDColor::RGB24(0xFEB834) },
  { KDColor::RGB24(0xFDBC44), KDColor::RGB24(0xFFFBF0), KDColor::RGB24(0xFFFEFF), KDColor::RGB24(0xFFFEFF), KDColor::RGB24(0xFFFFFF), KDColor::RGB24(0xFFF7F2), KDColor::RGB24(0xFEB834), KDColor::RGB24(0xFEB836) },
  { KDColor::RGB24(0xFFE2AE), KDColor::RGB24(0xFCFFFD), KDColor::RGB24(0xFFD692), KDColor::RGB24(0xFFC65B), KDColor::RGB24(0xFFCE79), KDColor::RGB24(0xFCFCF0), KDColor::RGB24(0xFDB636), KDColor::RGB24(0xFFB635) },
  { KDColor::RGB24(0xFFF6E5), KDColor::RGB24(0xFFE1B0), KDColor::RGB24(0xFDB836), KDColor::RGB24(0xFDB735), KDColor::RGB24(0xFEB532), KDColor::RGB24(0xFFC459), KDColor::RGB24(0xFDB836), KDColor::RGB24(0xFFB933) }
};

enum class SpinnerStatus : uint8_t {
  Spinning,
  Hidden,
  Disabled
};

static SpinnerStatus s_spinnerStatus = SpinnerStatus::Disabled;
static constexpr int k_numberOfSpinnerPositions = 16;
static int s_spinnerPosition = 0;
static KDColor s_hiddenBuffer[2*k_spinnerHalfHeight][2*k_spinnerHalfWidth];

void setSpinner(bool activate) {
  s_spinnerStatus = activate ? SpinnerStatus::Hidden : SpinnerStatus::Disabled;
}

void spin() {
  // Update spinner state
  if (s_spinnerStatus != SpinnerStatus::Spinning) {
    TIM2.ARR()->set(k_spinningDelay);
    s_spinnerStatus = SpinnerStatus::Spinning;
    Ion::Device::Display::pullRect(KDRect(k_spinnerX, k_spinnerY, 2 * k_spinnerHalfWidth, 2 * k_spinnerHalfHeight), reinterpret_cast<KDColor *>(s_hiddenBuffer));
  }
  s_spinnerPosition = (s_spinnerPosition + 1) % k_numberOfSpinnerPositions;

  static constexpr int k_numberOfQuadrants = 4;
  /* Each quadrant displays the four steps alternatively. Quadrant are indexed Left to right top to bottom*/
  KDColor buffer[k_spinnerHalfHeight][k_spinnerHalfWidth];
  KDColor * steps[k_numberOfQuadrants] = {reinterpret_cast<KDColor *>(s_spinnerFirstStepPixels), reinterpret_cast<KDColor *>(s_spinnerSecondStepPixels), reinterpret_cast<KDColor *>(s_spinnerThirdStepPixels), reinterpret_cast<KDColor *>(s_spinnerFourthStepPixels)};
  int movingQuandrantIndex = s_spinnerPosition/4;
  for (int q = 0; q < k_numberOfQuadrants; q++) {
    KDRect rect(k_spinnerX + (q == 1 || q == 2)*k_spinnerHalfWidth,
                k_spinnerY + (q == 1 || q == 3)*k_spinnerHalfHeight,
                k_spinnerHalfWidth,
                k_spinnerHalfHeight);
    KDColor * pixels;
    if (q == movingQuandrantIndex) {
      // Select the right step to display (the order is reversed for odd quadrant)
      int stepIndex = s_spinnerPosition % 4;
      pixels = steps[q % 2 ? 3 - stepIndex : stepIndex];
    } else {
      pixels = reinterpret_cast<KDColor *>(s_spinnerFullPixels);
    }
    // Flip the image (axial or central symmetry depending on quadrant)
    for (int i = 0; i < k_spinnerHalfHeight; i++) {
      for (int j = 0; j < k_spinnerHalfWidth; j++) {
        int newJ = q == 0 || q == 3 ? j : k_spinnerHalfWidth - 1 - j;
        int newI = q == 0 || q == 2 ? i : k_spinnerHalfHeight - 1 - i;
        buffer[i][j] = pixels[newI * k_spinnerHalfWidth + newJ];
      }
    }
    Ion::Device::Display::pushRect(rect, reinterpret_cast<KDColor *>(buffer));
  }
}

void hideSpinner() {
  TIM2.ARR()->set(k_stallingDelay);
  Ion::Device::Display::pushRect(KDRect(k_spinnerX, k_spinnerY, 2 * k_spinnerHalfWidth, 2 * k_spinnerHalfHeight), reinterpret_cast<KDColor *>(s_hiddenBuffer));
  s_spinnerStatus = SpinnerStatus::Hidden;
}

void pauseStallingTimer() {
  TIM2.stop();
}

void resetStallingTimer() {
  TIM2.launch();
  if (s_spinnerStatus == SpinnerStatus::Spinning) {
    /* Hide the spinner and reset the delay if the spinner was previously
     * displayed. */
    hideSpinner();
  }
}

void stall() {
  // Clear update interrupt flag
  TIM2.SR()->setUIF(false);

  /* Flush keyboard state to push preemptive state only, to avoid delayed
   * reaction. HandlePreemption will flush it again if necessary. */
  Keyboard::Queue::sharedQueue()->flush(false);
  Keyboard::Queue::sharedQueue()->push(Ion::Events::sPreemtiveState);

  if (Ion::Events::handlePreemption(true)) {
    return;
  }

  if (s_spinnerStatus != SpinnerStatus::Disabled) {
    spin();
    // TODO: stalling when no User/System checkpoint is set could lead to checkout the home
    // TODO: should we shutdown any interruption here to be sure to complete our drawing
    // TODO: draw a hourglass or spinning circle?
  }
}

}
}
}
