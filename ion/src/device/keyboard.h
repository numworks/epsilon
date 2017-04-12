#ifndef ION_DEVICE_KEYBOARD_H
#define ION_DEVICE_KEYBOARD_H

#include <ion/keyboard.h>
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

}
}
}

#endif
