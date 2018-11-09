#ifndef ION_DEVICE_KEYBOARD_H
#define ION_DEVICE_KEYBOARD_H

#include <ion/keyboard.h>
#include <ion.h>
#include "regs/regs.h"

namespace Ion {
namespace Keyboard {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+--------------------
 *  PC0 | Keyboard column 1 | Input, pulled up
 *  PC1 | Keyboard column 2 | Input, pulled up
 *  PC2 | Keyboard column 3 | Input, pulled up
 *  PC3 | Keyboard column 4 | Input, pulled up
 *  PC4 | Keyboard column 5 | Input, pulled up
 *  PC5 | Keyboard column 6 | Input, pulled up
 *  PE0 | Keyboard row A    | Output, open drain
 *  PE1 | Keyboard row B    | Output, open drain
 *  PE2 | Keyboard row C    | Output, open drain
 *  PE3 | Keyboard row D    | Output, open drain
 *  PE4 | Keyboard row E    | Output, open drain
 *  PE5 | Keyboard row F    | Output, open drain
 *  PE6 | Keyboard row G    | Output, open drain
 *  PE7 | Keyboard row H    | Output, open drain
 *  PE8 | Keyboard row I    | Output, open drain
 */

void init();
void shutdown();

constexpr GPIO RowGPIO = GPIOE;
constexpr uint8_t numberOfRows = 9;
constexpr uint8_t RowPins[numberOfRows] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

constexpr GPIO ColumnGPIO = GPIOC;
constexpr uint8_t numberOfColumns = 6;
constexpr uint8_t ColumnPins[numberOfColumns] = {0, 1, 2, 3, 4, 5};

inline uint8_t rowForKey(Key key) {
  return (int)key/numberOfColumns;
}
inline uint8_t columnForKey(Key key) {
  return (int)key%numberOfColumns;
}

inline void activateRow(uint8_t row) {
  /* In open-drain mode, a 0 in the register drives the pin low, and a 1 lets
   * the pin floating (Hi-Z). So we want to set the current row to zero and all
   * the others to 1. */
  uint16_t rowState = ~(1<<row);

  // TODO: Assert pin numbers are sequentials and dynamically find 9 and 0
  Device::RowGPIO.ODR()->setBitRange(9, 0, rowState);

  // TODO: 100 us seems to work, but wasn't really calculated
  usleep(100);
}

inline bool columnIsActive(uint8_t column) {
  return !(Device::ColumnGPIO.IDR()->getBitRange(column,column));
}

}
}
}

#endif
