#ifndef PLATFORM_FX9DKBD_H
#define PLATFORM_FX9DKBD_H 1

#include <stdint.h>

/* This is the FX9DKBD driver.
 *
 * This keyboard is actually a matrix of 50 keys that are connected over GPIOs.
 *
 * This matrix is laid out in a typicall row/columns fasion. By writing a given
 * value to the "rows", one can figure out which "columns" had a key pressed and
 * therefore deduce which keys are down and which are up.
 *
 * This driver expects a function that will write a value to the "columns" GPIOs
 * and another to read the value of the "row" GPIOs.
 *
 * In the current incarnation, the wires are numbered 0 to 15, 0 being the blue
 * wire. Here are how they are wired:
 * Wire 0 = ON button
 * Rows: Wires 4, 5, 8, 9, 11, 12 and 14. Those pins are written to.
 * Columns: Wires 1, 2, 3, 6, 7, 10, 13 and 15. Those pins are read from.
 */

typedef struct {
  void (*row_write)(uint8_t row);
  uint8_t (*column_read)();
} fx92kbd_t;

typedef enum {
  FX92_KEY_CUBE = 1,
  FX92_KEY_DOWN = 2,
  FX92_KEY_SQUARE = 4,
  FX92_KEY_LEFT = 5,
  FX92_KEY_OPTN = 6,
  FX92_KEY_TAN = 8,
  FX92_KEY_COS = 9,
  FX92_KEY_SIN = 10,
  FX92_KEY_ZERO = 11,
  FX92_KEY_EUCLID = 12,
  FX92_KEY_FRAC = 13,
  FX92_KEY_SIMP = 14,
  FX92_KEY_DIVISION = 17,
  FX92_KEY_MULTIPLICATION = 18,
  FX92_KEY_REP = 19,
  FX92_KEY_FIVE = 20,
  FX92_KEY_SIX = 21,
  FX92_KEY_FOUR = 22,
  FX92_KEY_AC = 25,
  FX92_KEY_SUPPR = 26,
  FX92_KEY_x10x = 27,
  FX92_KEY_EIGHT = 28,
  FX92_KEY_NINE = 29,
  FX92_KEY_SEVEN = 30,
  FX92_KEY_MENU = 33,
  FX92_KEY_RIGHT = 34,
  FX92_KEY_ALPHA = 36,
  FX92_KEY_UP = 37,
  FX92_KEY_SECOND = 38,
  FX92_KEY_MINUS = 41,
  FX92_KEY_PLUS = 42,
  FX92_KEY_EXE = 43,
  FX92_KEY_TWO = 44,
  FX92_KEY_THREE = 45,
  FX92_KEY_ONE = 46,
  FX92_KEY_M_PLUS = 48,
  FX92_KEY_S_D = 49,
  FX92_KEY_PAREN_RIGHT = 50,
  FX92_KEY_COMMA = 51,
  FX92_KEY_DMS = 52,
  FX92_KEY_PAREN_LEFT = 53,
  FX92_KEY_STO = 54,
  FX92_KEY_LN = 56,
  FX92_KEY_LOG = 57,
  FX92_KEY_X = 58,
  FX92_KEY_Y = 60,
  FX92_KEY_EQUAL = 61,
  FX92_KEY_CALC = 62,
  FX92_KEY_EXPONENT = 0,
  FX92_KEY_NONE = 0,
} fx92kbd_key_t;

void fx92kbd_initialize(fx92kbd_t * controller);
fx92kbd_key_t fx92kbd_getkey(fx92kbd_t * controller);

#endif
