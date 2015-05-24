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
#include <platform/fx92kbd/fx92kbd.h>

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

static fx92kbd_t keyboard = {
  .row_write = row_write,
  .column_read = column_read
};

// FIXME: Delete
static int delay(int usec) {
  for (int i=0; i<usec; i++) {
  }
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

  fx92kbd_initialize(&keyboard);

  fx92kbd_key_t key = FX92_KEY_NONE;
  while (1) {
    key = fx92kbd_getkey(&keyboard);
  }
}
