#ifndef ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_KEYBOARD_H
#define ION_DEVICE_N0100_SHARED_DRIVERS_CONFIG_KEYBOARD_H

#include <config/clocks.h>
#include <regs/regs.h>

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
 *
 * The keyboard is a matrix that is laid out as follow:
 *
 * -+------+------+------+------+------+------+
 *  | K_A1 | K_A2 | K_A3 | K_A4 | K_A5 | K_A6 |
 * -+------+------+------+------+------+------+
 *  | K_B1 | K_B2 |      |      |      |      |
 * -+------+------+------+------+------+------+
 *  | K_C1 | K_C2 | K_C3 | K_C4 | K_C5 | K_C6 |
 * -+------+------+------+------+------+------+
 *  | K_D1 | K_D2 | K_D3 | K_D4 | K_D5 | K_D6 |
 * -+------+------+------+------+------+------+
 *  | K_E1 | K_E2 | K_E3 | K_E4 | K_E5 | K_E6 |
 * -+------+------+------+------+------+------+
 *  | K_F1 | K_F2 | K_F3 | K_F4 | K_F5 |      |
 * -+------+------+------+------+------+------+
 *  | K_G1 | K_G2 | K_G3 | K_G4 | K_G5 |      |
 * -+------+------+------+------+------+------+
 *  | K_H1 | K_H2 | K_H3 | K_H4 | K_H5 |      |
 * -+------+------+------+------+------+------+
 *  | K_I1 | K_I2 | K_I3 | K_I4 | K_I5 |      |
 * -+------+------+------+------+------+------|
 */

#define KEYBOARD_CONFIG_ROWS_CONSECUTIVE 1

namespace Ion {
namespace Device {
namespace Keyboard {
namespace Config {

using namespace Regs;

constexpr GPIO RowGPIO = GPIOE;
constexpr uint8_t numberOfRows = 9;
constexpr uint8_t RowPins[numberOfRows] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

constexpr GPIO ColumnGPIO = GPIOC;
constexpr uint8_t numberOfColumns = 6;
constexpr uint8_t ColumnPins[numberOfColumns] = {0, 1, 2, 3, 4, 5};

/* Undefined keys numbers are: 8, 9, 10, 11, 35, 41, 47 and 53
 * Therefore we want to make sure those bits are forced to zero in
 * whatever value we return. */
inline uint64_t ValidKeys(uint64_t state) {
  return state & 0x1F7DF7FFFFF0FF;
}

constexpr int DebouncingDelay = 10;
constexpr int PollDelay = Clocks::Config::AHBLowFrequencyPrescaler * 100;

}
}
}
}

#endif
