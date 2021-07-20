#include <drivers/config/clocks.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard.h>
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
static constexpr int k_pollDelay = 100;

static void stopPollTimer() {
  TIM5.SR()->setUIF(false);
  TIM5.CR1()->setCEN(false);
}

void initTimer() {
  TIM4.PSC()->set(Clocks::Config::APB1TimerFrequency-1);
  TIM4.DIER()->setUIE(true);
  TIM4.ARR()->set(k_debouncingDelay);

  TIM5.PSC()->set(Clocks::Config::APB1TimerFrequency-1);
  TIM5.DIER()->setUIE(true);
  TIM5.ARR()->set(k_pollDelay);
  stopPollTimer(); // only poll while a key is pressed
}

void shutdownTimer() {
  TIM5.DIER()->setUIE(false);
  TIM5.CR1()->setCEN(false);

  TIM4.DIER()->setUIE(false);
  TIM4.CR1()->setCEN(false);
}

static constexpr int interruptionISRIndex[] = {6, 7, 8, 9, 10, 23, 30, 50};

void initInterruptions() {
  /* Init EXTI interrupts (corresponding to keyboard column pins) and TIM4
   * and TIM5 interrupts . */
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

static void launchPollTimer() {
  TIM5.CNT()->set(0);
  TIM5.CR1()->setCEN(true);
}

static bool sBouncing = false;

void debounce() {
  sBouncing = false;
  TIM4.SR()->setUIF(false);
  TIM4.CR1()->setCEN(false);
}

State sState(0);

/* Keystroke detection uses the EXTI line, which interrupts the execution
 * whenever a rising edge appears on a column. However, if a key is held down,
 * pressing another key in the same column will not produce an edge and the
 * keyboard state is not queued.
 * Therefore, as long as a key is held down, we need to periodically poll the
 * keyboard to detect all key presses. To keep the timer running as little as
 * possible, we only activate it when Shift or Alpha are pressed, as those are
 * the only "combining" keys we really need to support. */

void poll() {
  stopPollTimer();
  State state = Keyboard::scan();
  if (state.keyDown(Key::Shift) || state.keyDown(Key::Alpha) ) {
    launchPollTimer();
  }
  /* OnOff, Home and Back are the only keyboard keys which are preemptive.
   * The states which doesn't involve one of these keys down are pushed on a
   * queue and depile one at a time.
   * If the device is stalling, we do not queue the event to avoid a delayed
   * reaction.
   * */
  if (state != sState) {
    Events::setPendingKeyboardStateIfPreemtive(state);
    Queue::sharedQueue()->push(state);
    sState = state;
  }
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
    poll();
    launchDebounceTimer();
  }
}

}
}
}
