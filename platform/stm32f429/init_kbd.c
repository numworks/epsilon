/* Keyboard initialization code
 *
 * E0 = ON button
 * Columns: E1, E2, E3, E6, E7, E10, E13, E15
 * Rows = E4, E5, E8, E9, E11, E12, E14
 *
 */

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include "registers.h"
#include "init_kbd.h"
#include <platform/fx92kbd/fx92kbd.h>
#include <platform/platform.h>

// The row pins are driven high or low in software.
static gpio_pin_t row_pins[] = {
  {GPIOE, 1}, {GPIOE, 2}, {GPIOE, 3}, {GPIOE, 6},
  {GPIOE, 7}, {GPIOE, 10}, {GPIOE, 13}, {GPIOE, 15}
};

// The columns pins are read.
static gpio_pin_t column_pins[] = {
  {GPIOE, 4}, {GPIOE, 5}, {GPIOE, 8}, {GPIOE, 9},
  {GPIOE, 11}, {GPIOE, 12}, {GPIOE, 14}
};

static void row_write(uint8_t row) {
  size_t row_length = sizeof(row_pins)/sizeof(row_pins[0]);
  for (int i=0; i<row_length; i++) {
    gpio_pin_t pin = row_pins[i];
    uint8_t bit_value = (((1<<i) & row) != 0);
    REGISTER_SET_VALUE(GPIO_ODR(pin.group), ODR(pin.number), bit_value);
  }
}

static uint16_t column_read() {
  uint16_t output_value = 0;
  size_t column_length = sizeof(column_pins)/sizeof(column_pins[0]);
  for (int i=0; i<column_length; i++) {
    gpio_pin_t pin = column_pins[i];
    if (GPIO_IDR(pin.group) & (1 << pin.number)) {
      output_value |= (1<<i);
    }
  }
  return output_value;
}

void init_kbd() {
  // We are using group E. Let's enable its clock.
  RCC_AHB1ENR |= GPIOEEN;

  // The rows are outputs.
  size_t row_count = sizeof(row_pins)/sizeof(row_pins[0]);
  for (int i=0; i<row_count; i++) {
    gpio_pin_t pin = row_pins[i];
    REGISTER_SET_VALUE(GPIO_MODER(pin.group), MODER(pin.number), GPIO_MODE_OUTPUT);
  }

  /* And the columns are inputs. We want them to be pulled-down: indeed, we want
   * to read a value of 0 when no row is pulling the column high. */
  size_t column_count = sizeof(column_pins)/sizeof(column_pins[0]);
  for (int i=0; i<column_count; i++) {
    gpio_pin_t pin = column_pins[i];
    REGISTER_SET_VALUE(GPIO_MODER(pin.group), MODER(pin.number), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(pin.group), PUPDR(pin.number), GPIO_PUPD_PULL_DOWN);
  }

  fx92kbd_t * keyboard = &(Platform.keyboard);
  keyboard->row_write = row_write;
  keyboard->column_read = column_read;

  fx92kbd_initialize(keyboard);
}

char charFromKey[] = {
  ' ', // FX92_KEY_NONE = 0
  ' ', // FX92_KEY_CUBE = 1,
  ' ', // FX92_KEY_DOWN = 2,
  ' ',
  ' ', // FX92_KEY_SQUARE = 4,
  ' ', // FX92_KEY_LEFT = 5,
  ' ', // FX92_KEY_OPTN = 6,
  ' ',
  ' ', // FX92_KEY_TAN = 8,
  ' ', // FX92_KEY_COS = 9,
  ' ', // FX92_KEY_SIN = 10,
  '0', // FX92_KEY_ZERO = 11,
  ' ', // FX92_KEY_EUCLID = 12,
  ' ', // FX92_KEY_FRAC = 13,
  ' ', // FX92_KEY_SIMP = 14,
  ' ',
  ' ',
  '/', // FX92_KEY_DIVISION = 17,
  '*', // FX92_KEY_MULTIPLICATION = 18,
  ' ', // FX92_KEY_REP = 19,
  '5', // FX92_KEY_FIVE = 20,
  '6', // FX92_KEY_SIX = 21,
  '4', // FX92_KEY_FOUR = 22,
  ' ',
  ' ',
  ' ', // FX92_KEY_AC = 25,
  ' ', // FX92_KEY_SUPPR = 26,
  ' ', // FX92_KEY_x10x = 27,
  '8', // FX92_KEY_EIGHT = 28,
  '9', // FX92_KEY_NINE = 29,
  '7', // FX92_KEY_SEVEN = 30,
  ' ',
  ' ',
  ' ', // FX92_KEY_MENU = 33,
  ' ', // FX92_KEY_RIGHT = 34,
  ' ',
  ' ', // FX92_KEY_ALPHA = 36,
  ' ', // FX92_KEY_UP = 37,
  ' ', // FX92_KEY_SECOND = 38,
  ' ',
  ' ',
  '-', // FX92_KEY_MINUS = 41,
  '+', // FX92_KEY_PLUS = 42,
  'X', // FX92_KEY_EXE = 43,
  '2', // FX92_KEY_TWO = 44,
  '3', // FX92_KEY_THREE = 45,
  '1', // FX92_KEY_ONE = 46,
  ' ',
  ' ', // FX92_KEY_M_PLUS = 48,
  ' ', // FX92_KEY_S_D = 49,
  ' ', // FX92_KEY_PAREN_RIGHT = 50,
  ' ', // FX92_KEY_COMMA = 51,
  ' ', // FX92_KEY_DMS = 52,
  ' ', // FX92_KEY_PAREN_LEFT = 53,
  ' ', // FX92_KEY_STO = 54,
  ' ',
  ' ', // FX92_KEY_LN = 56,
  ' ', // FX92_KEY_LOG = 57,
  ' ', // FX92_KEY_X = 58,
  ' ',
  ' ', // FX92_KEY_Y = 60,
  ' ', // FX92_KEY_EQUAL = 61,
  ' ', // FX92_KEY_CALC = 62,
  ' ', // FX92_KEY_EXPONENT = 0,
  ' ', // FX92_KEY_NONE = 0,
};

char getc() {
  char character = ' ';
  while (character == ' ') {
    fx92kbd_key_t key = fx92kbd_getkey(&Platform.keyboard);
    character = charFromKey[key];
  }
  return character;
}
