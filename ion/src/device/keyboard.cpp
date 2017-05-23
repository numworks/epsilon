/* Keyboard initialization code
 *
 * The job of this code is to implement the "ion_key_state" function.
 *
 * The keyboard is a matrix that is laid out as follow:
 *
 *      |  PC0 |  PC1 | PC2  | PC3  | PC4  | PC5  |
 * -----+------+------+------+------+------+------+
 *  PE0 | K_A1 | K_A2 | K_A3 | K_A4 | K_A5 | K_A6 |
 * -----+------+------+------+------+------+------+
 *  PE1 | K_B1 | K_B2 |      |      |      |      |
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

#include <ion.h>
#include "keyboard.h"

// Public Ion::Keyboard methods

namespace Ion {
namespace Keyboard {

State scan() {
  uint64_t state = 0;

  for (uint8_t i=0; i<Device::numberOfRows; i++) {
    /* In open-drain mode, a 0 in the register drives the pin low, and a 1 lets
     * the pin floating (Hi-Z). So we want to set the current row to zero and
     * all the others to 1. */
    uint16_t rowState = ~(1<<(Device::numberOfRows-1-i));

    // TODO: Assert pin numbers are sequentials and dynamically find 9 and 0
    Device::RowGPIO.ODR()->setBitRange(9, 0, rowState);

    // TODO: 100 us seems to work, but wasn't really calculated
    usleep(100);

    // TODO: Assert pin numbers are sequentials and dynamically find 8 and 0
    uint8_t column = Device::ColumnGPIO.IDR()->getBitRange(5,0);

    /* The key is down if the input is brought low by the output. In other
     * words, we want to return true if the input is low (false). So we need to
     * append 6 bits of (not column) to state. */
    state = (state << 6) | (~column & 0x3F);
  }

  /* Last but not least, keys number 8, 9, 10, 11, 35, 41, 47 and 53 are not
   * defined. Therefore we want to make sure those bits are forced to zero in
   * whatever value we return. */
  state = state & 0x1F7DF7FFFFF0FF;

  return State(state);
}

}
}

// Private Ion::Keyboard::Device methods

namespace Ion {
namespace Keyboard {
namespace Device {

void init() {
  for (uint8_t i=0; i<numberOfRows; i++) {
    uint8_t pin = RowPins[i];
    RowGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Output);
    RowGPIO.OTYPER()->setType(pin, GPIO::OTYPER::Type::OpenDrain);
  }

  for (uint8_t i=0; i<numberOfColumns; i++) {
    uint8_t pin = ColumnPins[i];
    ColumnGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Input);
    ColumnGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::Up);
  }
}

void shutdown() {
  for (uint8_t i=0; i<numberOfRows; i++) {
    uint8_t pin = RowPins[i];
    RowGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Analog);
    RowGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::None);
  }

  for (uint8_t i=0; i<numberOfColumns; i++) {
    uint8_t pin = ColumnPins[i];
    ColumnGPIO.MODER()->setMode(pin, GPIO::MODER::Mode::Analog);
    ColumnGPIO.PUPDR()->setPull(pin, GPIO::PUPDR::Pull::None);
  }
}

}
}
}
