/* Keyboard initialization code
 *
 * Our role here is to implement the "ion_key_state" function.
 * The keyboard is a matrix that is as plugged as follow:
 *
 *     |  PA8 |  PA9 | PA10 | PA11 | PA12 |
 * ----+------+------+------+------+------+
 * PA0 | K_A1 | K_A2 | K_A3 | K_A4 | K_A5 |
 * ----+------+------+------+------+------+
 * PA1 | K_B1 | K_B2 | K_B3 | K_B4 | K_B5 |
 * ----+------+------+------+------+------+
 * PA2 |      |      | K_C1 | K_C2 | K_C3 |
 * ----+------+------+------+------+------+
 * PA3 | K_D1 | K_D2 | K_D3 | K_D4 | K_D5 |
 * ----+------+------+------+------+------+
 * PA4 | K_E1 | K_E2 | K_E3 | K_E4 | K_E5 |
 * ----+------+------+------+------+------+
 * PA5 | K_F1 | K_F2 | K_F3 | K_F4 | K_F5 |
 * ----+------+------+------+------+------+
 * PA6 | K_G1 | K_G2 | K_G3 | K_G4 | K_G5 |
 * ----+------+------+------+------+------+
 *
 *  We decide to drive the rows (PA0-6) and read the columns (PA8-12).
 *
 *  To avoid short-circuits, the pins A0-A6 will not be standard outputs but
 *  only open-drain. Open drain can either drive low or let it float. Otherwise,
 *  when a user presses multiple keys, a shortcut between rows could happen,
 *  which could trigger a short circuit between an output driving high and
 *  another driving low.
 *
 *  If the outputs are open-drain, this means that the input must be pulled up.
 *  So if the input reads "1", this means the key is in fact *not* pressed, and
 *  if it reads "0" it means that there's a short to an open-drain output. Which
 *  means the corresponding key is pressed.
 */

#include <ion.h>
#include "registers/registers.h"

// We'll make the assertion that the row and column pins are contiguous
#define ROW_PIN_START 0
#define ROW_PIN_END 6
#define COLUMN_PIN_START 8
#define COLUMN_PIN_END 12

static inline uint8_t row_for_key(ion_key_t key) {
  return key/5;
}
static inline uint8_t column_for_key(ion_key_t key) {
  return key%5;
}

bool ion_key_down(ion_key_t key) {
  /* Drive the corresponding row low, and let all the others float.
   * Note: In open-drain mode, a 0 in the register drives low, and a 1 lets the
   * pin in Hi-Z (floating). */
  uint32_t output_mask = (((uint32_t)1 << (ROW_PIN_END-ROW_PIN_START+1)) - 1) << ROW_PIN_START;
  uint32_t previous_odr = GPIO_ODR(GPIOA);
  uint32_t new_odr = ~((uint32_t)1 << (row_for_key(key)+ROW_PIN_START)) & output_mask;

  if (new_odr != previous_odr) {
    GPIO_ODR(GPIOA) = new_odr;
    // We changed the outputs, give the hardware some time to react to this change
    // FIXME: Real delay!
    for (int i=0;i<1000; i++) {
    }
  }

  // Read the input of the proper column
  uint32_t input = (GPIO_IDR(GPIOA) & (1 << (column_for_key(key)+COLUMN_PIN_START)));

  /* The key is down if the input is brought low by the output. In other words,
   * we want to return "true" (1) if the input is low (0). */
  return (input == 0);
}

void init_keyboard() {
  /* We are using GPIO group A, which live on the AHB1 bus. Let's start by
   * enabling its clock. */
  RCC_AHB1ENR |= GPIOAEN;

  // Configure the row pins as open-drain outputs
  for (int pin=ROW_PIN_START; pin<=ROW_PIN_END; pin++) {
    REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(pin), GPIO_MODE_OUTPUT);
    REGISTER_SET_VALUE(GPIO_OTYPER(GPIOA), OTYPER(pin), GPIO_OTYPE_OPEN_DRAIN);
  }

  // Configure the column as are pulled-up inputs
  for (int pin=COLUMN_PIN_START; pin<=COLUMN_PIN_END; pin++) {
    REGISTER_SET_VALUE(GPIO_MODER(GPIOA), MODER(pin), GPIO_MODE_INPUT);
    REGISTER_SET_VALUE(GPIO_PUPDR(GPIOA), PUPDR(pin), GPIO_PUPD_PULL_UP);
  }
}
