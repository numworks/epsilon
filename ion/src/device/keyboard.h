#ifndef ION_DEVICE_KEYBOARD_H
#define ION_DEVICE_KEYBOARD_H

#include <ion/keyboard.h>
#include "regs/regs.h"

namespace Ion {
namespace Keyboard {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+--------------------
 *  PB1 | Keyboard row A    | Output, open drain
 *  PB2 | Keyboard row B    | Output, open drain
 *  PB3 | Keyboard row C    | Output, open drain
 *  PB4 | Keyboard row D    | Output, open drain
 *  PB5 | Keyboard row E    | Output, open drain
 *  PB6 | Keyboard row F    | Output, open drain
 *  PB7 | Keyboard row G    | Output, open drain
 *  PB8 | Keyboard row H    | Output, open drain
 *  PB9 | Keyboard row I    | Output, open drain
 * PB10 | Keyboard row J    | Output, open drain
 *  PC0 | Keyboard column 1 | Input, pulled up
 *  PC1 | Keyboard column 2 | Input, pulled up
 * PC13 | Keyboard column 3 | Input, pulled up
 * PC14 | Keyboard column 4 | Input, pulled up
 * PC15 | Keyboard column 5 | Input, pulled up
 */

void init();

constexpr GPIO RowGPIO = GPIOB;
constexpr uint8_t numberOfRows = 10;
constexpr uint8_t RowPins[numberOfRows] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

constexpr GPIO ColumnGPIO = GPIOC;
constexpr uint8_t numberOfColumns = 5;
constexpr uint8_t ColumnPins[numberOfColumns] = {0, 1, 13, 14, 15};

inline uint8_t rowForKey(Key key) {
  return (int)key/numberOfColumns;
}
inline uint8_t columnForKey(Key key) {
  return (int)key%numberOfColumns;
}

void generateWakeUpEventForKey(Key k);

}
}
}

#endif
