/* Keyboard initialization code
 *
 * The job of this code is to implement the "ion_key_state" function.
 *
 * The keyboard is a matrix that is laid out as follow:
 * (K_B2 and K_B3 are respectively specific to N0100 and N0110
 *
 *      |  PC0 |  PC1 | PC2  | PC3  | PC4  | PC5  |
 * -----+------+------+------+------+------+------+
 *  PE0 | K_A1 | K_A2 | K_A3 | K_A4 | K_A5 | K_A6 |
 * -----+------+------+------+------+------+------+
 *  PE1 | K_B1 |(K_B2)|(K_B3)|      |      |      |
 * -----+------+------+------+------+------+------+
 *  PE2 | K_C1 | K_C2 | K_C3 | K_C4 | K_C5 | K_C6 |
 * -----+------+------+------+------+------+------+
 *  PE3 | K_D1 | K_D2 | K_D3 | K_D4 | K_D5 | K_D6 |
 * -----+------+------+------+------+------+------+
 *  PE4 | K_E1 | K_E2 | K_E3 | K_E4 | K_E5 | K_E6 |
 * -----+------+------+------+------+------+------+
 *  PE5 | K_F1 | K_F2 | K_F3 | K_F4 | K_F5 |      |
 * -----+------+------+------+------+------+------+
 *  PE6 | K_G1 | K_G2 | K_G3 | K_G4 | K_G5 |      |
 * -----+------+------+------+------+------+------+
 *  PE7 | K_H1 | K_H2 | K_H3 | K_H4 | K_H5 |      |
 * -----+------+------+------+------+------+------+
 *  PE8 | K_I1 | K_I2 | K_I3 | K_I4 | K_I5 |      |
 * -----+------+------+------+------+------+------|
 *
 *  We decide to drive the rows (PE0-8) and read the columns (PC0-5).
 *
 *  To avoid short-circuits, the pins E0-E8 will not be standard outputs but
 *  only open-drain. Open drain means the pin is either driven low or left
 *  floating.
 *  When a user presses multiple keys, a connection between two rows can happen.
 *  If we don't use open drain outputs, this situation could trigger a short
 *  circuit between an output driving high and another driving low.
 *
 *  If the outputs are open-drain, this means that the input must be pulled up.
 *  So if the input reads "1", this means the key is in fact *not* pressed, and
 *  if it reads "0" it means that there's a short to an open-drain output. Which
 *  means the corresponding key is pressed.
 */

#include <kernel/drivers/keyboard.h>
#include <drivers/config/clocks.h>
#include <kernel/drivers/keyboard_queue.h>

namespace Ion {
namespace Device {
namespace Keyboard {

State scan() {
  uint64_t state = 0;

  for (uint8_t i=0; i<Config::numberOfRows; i++) {
    activateRow(Config::numberOfRows-1-i);

    // TODO: Assert pin numbers are sequentials and dynamically find 8 and 0
    uint8_t columns = Config::ColumnGPIO.IDR()->getBitRange(5,0);

    /* The key is down if the input is brought low by the output. In other
     * words, we want to return true if the input is low (false). So we need to
     * append 6 bits of (not columns) to state. */
    state = (state << 6) | (~columns & 0x3F);
  }
  // Detecting interruption requires all row to be pulled-down
  activateAllRows();

  // Make sure undefined key bits are forced to zero in the return value
  state = Config::ValidKeys(state);

  return State(state);
}

using namespace Regs;

void initInterruptions() {
  // Init interruption
  /*
   * GPIO Pin Number|EXTICR1|EXTICR2|EXTICR3| Interruption on
   * ---------------+-------+-------+-------+-------------------------
   *        0       |   C   | ***** | ***** | Both edges GPIO C pin 0
   *        1       |   C   | ***** | ***** | Both edge GPIO C pin 1
   *        2       |   C   | ***** | ***** | Both edge GPIO C pin 2
   *        3       |   C   | ***** | ***** | Both edge GPIO C pin 3
   *        4       | ***** |   C   | ***** | Both edge GPIO C pin 4
   *        5       | ***** |   C   | ***** | Both edge GPIO C pin 5
   *
   */
  for (uint8_t i=0; i<Config::numberOfColumns; i++) {
    uint8_t pin = Config::ColumnPins[i];
    SYSCFG.EXTICR()->setEXTI(pin, Keyboard::Config::ColumnGPIO);
    EXTI.IMR()->set(pin, true);
    EXTI.FTSR()->set(pin, true);
    EXTI.RTSR()->set(pin, true);
  }
}
/* We want to prescale the timer to be able to set the auto-reload in
 * milliseconds. However, since the prescaler range is 2^16-1, we use a factor
 * not to overflow PSC. */
static constexpr int k_prescalerFactor = 4;

void initTimer() {
  TIM4.PSC()->set(Clocks::Config::APB1TimerFrequency*1000/k_prescalerFactor-1);
}

void init(bool activateInterruptions) {
#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  /* PA0 pin is also used as the wake up pin of the standby mode. It has to be
   * unable to be used in output mode, open-drain for the keyboard. */
  PWR.CSR2()->setEWUP1(false); // Disable PA0 as wakeup pin
  PWR.CR2()->setCWUPF1(true); // Clear wakeup pin flag for PA0
#endif

  for (uint8_t i=0; i<Config::numberOfRows; i++) {
    uint8_t pin = Config::RowPins[i];
    Config::RowGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Output);
    Config::RowGPIO.OTYPER()->setType(pin, GPIO::OTYPER::Type::OpenDrain);
  }

  for (uint8_t i=0; i<Config::numberOfColumns; i++) {
    uint8_t pin = Config::ColumnPins[i];
    Config::ColumnGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Input);
    Config::ColumnGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::Up);
  }

  if (activateInterruptions) {
    // Detecting interruption requires all row to be pulled-down
    activateAllRows();

    initTimer();
    initInterruptions();
  }
}

void shutdown() {
  for (uint8_t i=0; i<Config::numberOfRows; i++) {
    uint8_t pin = Config::RowPins[i];
    Config::RowGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Analog);
    Config::RowGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::None);
  }

  for (uint8_t i=0; i<Config::numberOfColumns; i++) {
    uint8_t pin = Config::ColumnPins[i];
    Config::ColumnGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Analog);
    Config::ColumnGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::None);
  }
}

void launchTimer(uint16_t ms) {
  TIM4.ARR()->set(ms*k_prescalerFactor);
  TIM4.DIER()->setUIE(true);
  TIM4.CR1()->setCEN(true);
}

void stopTimer() {
  TIM4.DIER()->setUIE(false);
  TIM4.CR1()->setCEN(false);
}

static bool sBouncing = false;

void debounce() {
  sBouncing = false;
  stopTimer();
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
    Queue::sharedQueue()->push(Keyboard::scan());
    launchTimer(10);
  }
}

}
}
}
