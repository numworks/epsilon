#include <kernel/drivers/keyboard.h>
#include <drivers/config/clocks.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard_queue.h>

namespace Ion {
namespace Device {
namespace Keyboard {

State popState() {
  if (Queue::sharedQueue()->isEmpty()) {
    return State(-1);
  }
  return Queue::sharedQueue()->pop();
}

using namespace Regs;

static constexpr int k_debouncingDelay = 10;

void initTimer() {
  TIM4.PSC()->set(Clocks::Config::APB1TimerFrequency-1);
  TIM4.DIER()->setUIE(true);
  TIM4.ARR()->set(k_debouncingDelay);
}

void shutdownTimer() {
  TIM4.DIER()->setUIE(false);
  TIM4.CR1()->setCEN(false);
}

static constexpr int interruptionISRIndex[] = {6, 7, 8, 9, 10, 23, 30};

void initInterruptions() {
  /* Init EXTI interrupts (corresponding to keyboard column pins) and TIM4
   * interrupt. */
  for (size_t i = 0; i < sizeof(interruptionISRIndex)/sizeof(int); i++) {
      // Flush pending interruptions
    while (NVIC.NVIC_ICPR()->getBit(interruptionISRIndex[i])) { // Read to force writing
      NVIC.NVIC_ICPR()->setBit(interruptionISRIndex[i], true);
    }
    // Enable interruptions
    NVIC.NVIC_ISER()->setBit(interruptionISRIndex[i], true);
  }

  initExtendedInterruptions();
  initTimer();
}

void shutdownInterruptions() {
  shutdownTimer();
  shutdownExtendedInterruptions();

  for (size_t i = 0; i < sizeof(interruptionISRIndex)/sizeof(int); i++) {
    // Disable interruptions
    NVIC.NVIC_ICER()->setBit(interruptionISRIndex[i], true);
  }
}

void init(bool fromBootloader, bool activateInterruptions) {
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  /* PA0 pin is also used as the wake up pin of the standby mode. It has to be
   * unable to be used in output mode, open-drain for the keyboard. */
  PWR.CSR2()->setEWUP1(false); // Disable PA0 as wakeup pin
  PWR.CR2()->setCWUPF1(true); // Clear wakeup pin flag for PA0
#endif

  // Bootloader has already initialized the GPIO
  if (!fromBootloader) {
    initGPIO();
  }

  if (activateInterruptions) {
    // Detecting interruption requires all row to be pulled-down
    activateAllRows();
    initInterruptions();
  }
}

void shutdown() {
  /* Disable interruption before shutting down the keyboard pins that would
   * trigger an interruption when changing the GPIO mode. */
  shutdownInterruptions();
  shutdownGPIO();
}

void launchDebounceTimer() {
  TIM4.CNT()->set(0);
  TIM4.CR1()->setCEN(true);
}

static bool sBouncing = false;

void debounce() {
  sBouncing = false;
  TIM4.SR()->setUIF(false);
  TIM4.CR1()->setCEN(false);
}

void handleInterruption() {
  for (uint8_t i=0; i<Config::numberOfColumns; i++) {
    uint8_t pin = Config::ColumnPins[i];
    if (EXTI.PR()->get(pin)) {
      EXTI.PR()->set(pin, true);
    }
  }
  if (!sBouncing) {
    sBouncing = true;
    State state = Keyboard::scan();
    /* OnOff, Home and Back are the only keyboard keys which are preemptive.
     * The states which doesn't involve one of these keys down are pushed on a
     * queue and depile one at a time.
     * If the device is stalling, we do not queue the event to avoid a delayed
     * reaction.
     * */
    if (!(Events::setPendingKeyboardStateIfPreemtive(state))) {
      Queue::sharedQueue()->push(state);
    }
    launchDebounceTimer();
  }
}

}
}
}
