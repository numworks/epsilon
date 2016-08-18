/* Keyboard initialization code
 *
 * The job of this code is to implement the "ion_key_state" function.
 *
 * The keyboard is a matrix that is laid out as follow:
 *
 *      |  PC0 |  PC1 | PC13 | PC14 | PC15 |
 * -----+------+------+------+------+------+
 *  PB1 | K_A1 | K_A2 | K_A3 | K_A4 | K_A5 |
 * -----+------+------+------+------+------+
 *  PB2 | K_B1 | K_B2 | K_B3 | K_B4 | K_B5 |
 * -----+------+------+------+------+------+
 *  PB3 | K_C1 | K_C2 | K_C3 | K_C4 | K_C5 |
 * -----+------+------+------+------+------+
 *  PB4 | K_D1 | K_D2 | K_D3 | K_D4 | K_D5 |
 * -----+------+------+------+------+------+
 *  PB5 | K_E1 | K_E2 | K_E3 | K_E4 | K_E5 |
 * -----+------+------+------+------+------+
 *  PB6 | K_F1 | K_F2 | K_F3 | K_F4 | K_F5 |
 * -----+------+------+------+------+------+
 *  PB7 | K_G1 | K_G2 | K_G3 | K_G4 | K_G5 |
 * -----+------+------+------+------+------+
 *  PB8 | K_H1 | K_H2 | K_H3 | K_H4 | K_H5 |
 * -----+------+------+------+------+------+
 *  PB9 | K_I1 | K_I2 | K_I3 | K_I4 | K_I5 |
 * -----+------+------+------+------+------+
 * PB10 | K_J1 | K_J2 | K_J3 | K_J4 | K_J5 |
 * -----+------+------+------+------+------+
 *
 *  We decide to drive the rows (PB1-10) and read the columns (PC0,1,13,14,15).
 *
 *  To avoid short-circuits, the pins B1-B10 will not be standard outputs but
 *  only open-drain. Open drain means the pin is either driven low or left
 *  floating.
 *  When a user presses multiple keys, a connection between two rows can happen.
 *  If we dont' use open drain outputs, this situation could trigger a short
 *  circuit between an output driving high and another driving low.
 *
 *  If the outputs are open-drain, this means that the input must be pulled up.
 *  So if the input reads "1", this means the key is in fact *not* pressed, and
 *  if it reads "0" it means that there's a short to an open-drain output. Which
 *  means the corresponding key is pressed.
 */

#include <ion.h>
#include "../registers/registers.h"

/* This driver expects that row pins are contiguous. It also expects that
 * column pins are contiguous too. This is not mandatory but it makes the code
 * a lot simpler. */

#define ROW_GPIO GPIOB
#define LOW_BIT_ROW 1
#define HIGH_BIT_ROW 10

#define COLUMN_GPIO GPIOC

static inline uint8_t row_for_key(ion_key_t key) {
  return key/5;
}
static inline uint8_t column_for_key(ion_key_t key) {
  return key%5;
}

bool ion_key_down(ion_key_t key) {
  /* Drive the corresponding row low, and let all the others float.
   * Note: In open-drain mode, a 0 in the register drives low, and a 1 lets the
   * pin floating (Hi-Z). */
  uint32_t new_row_value = ~((uint32_t)1 << row_for_key(key));
  uint32_t previous_row_value = REGISTER_GET_VALUE(GPIO_ODR(ROW_GPIO), ROW);

  if (new_row_value != previous_row_value) {
    REGISTER_SET_VALUE(GPIO_ODR(ROW_GPIO), ROW, new_row_value);
    // We changed the outputs, give the hardware some time to react to this change
    // FIXME: Real delay!
    for (volatile int i=0;i<1000; i++) {
    }
  }

  // Read the input of the proper column
  uint8_t column = column_for_key(key);
  uint8_t bit_for_column = (column <= 1 ? column : column+11);
  uint32_t input = (GPIO_IDR(COLUMN_GPIO) & (1 << bit_for_column));

  /* The key is down if the input is brought low by the output. In other words,
   * we want to return "true" (1) if the input is low (0). */
  return (input == 0);
}

void init_keyboard() {
  /* We are using GPIOs, let's start by enabling their clock. */
  /* Note: All GPIOs live on the AHB1 bus */
  RCC_AHB1ENR |= GPIO_EN(ROW_GPIO) | GPIO_EN(COLUMN_GPIO);

  // Configure the row pins as open-drain outputs
  for (int pin=LOW_BIT_ROW; pin<=HIGH_BIT_ROW; pin++) {
    REGISTER_SET_VALUE(GPIO_MODER(ROW_GPIO), MODER(pin), GPIO_MODE_OUTPUT);
    REGISTER_SET_VALUE(GPIO_OTYPER(ROW_GPIO), OTYPER(pin), GPIO_OTYPE_OPEN_DRAIN);
  }

  // Configure the column as are pulled-up inputs
    REGISTER_SET_VALUE(GPIO_MODER(COLUMN_GPIO), MODER(0), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(COLUMN_GPIO), PUPDR(0), GPIO_PUPD_PULL_UP);
    REGISTER_SET_VALUE(GPIO_MODER(COLUMN_GPIO), MODER(1), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(COLUMN_GPIO), PUPDR(1), GPIO_PUPD_PULL_UP);
    REGISTER_SET_VALUE(GPIO_MODER(COLUMN_GPIO), MODER(13), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(COLUMN_GPIO), PUPDR(13), GPIO_PUPD_PULL_UP);
    REGISTER_SET_VALUE(GPIO_MODER(COLUMN_GPIO), MODER(14), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(COLUMN_GPIO), PUPDR(14), GPIO_PUPD_PULL_UP);
    REGISTER_SET_VALUE(GPIO_MODER(COLUMN_GPIO), MODER(15), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(COLUMN_GPIO), PUPDR(15), GPIO_PUPD_PULL_UP);
}
