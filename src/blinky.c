long DataSectionTest = 0x1234;

void sleep(long delay);

int main(int argc, char * argv[]) {
  for (int i=0; i< DataSectionTest; i++) {
  }

  // We want to blink LEDs connected to GPIO pin G13 and G14
  // (this is documented in our board's PDF)
  //
  // GPIO are grouped by letter, and GPIO "G" live on the "AHB1" bus
  // (this is documented in the STM32F4 reference mnual, page 65)

  // More specifically, the boundary addresses for GPIOC are
  // 0x4002 1800 - 0x4002 1BFF
  //

  // Step 1 : Enable clock in RCC_AHBxENR
  // (RCC stands for "Reset and Clock Control)
  // Per doc (STM32F4 reference manual, page 266), we want to enable
  //  the 6th bit of RCC_AHB1ENR, which is at offset 0x30
  // The RCC register boundary address is 0x4002 3800 - 0x4002 3BFF
  // (per STM32F429xx datasheet)
  // In other words, we want to enable bit 6 of (0x40023830)

  long * RCCAHB1ENR = (long *)0x40023830;
  *RCCAHB1ENR |= (1 << 6); // Set 6th bit of RCCAHB1ENR

  // Step 2 : Configure the GPIO pin
  // Set the mode to general purpose output
  // GPIOG_MODER, bit 26 and 27 = pin13
  // GPIOG_MODER, bit 28 and 29 = pin14
  // A value of 0,0 for these two bits means input (reset state)
  // and 0,1 for these two bits means output (which we want)
  long * GPIOG_MODER = (long *)0x40021800; // GPIOC register

  *GPIOG_MODER &= ~(1 << 27); // Clear bit 27 of GPIOG_MODER
  *GPIOG_MODER |= (1 << 26); // Set bit 26 of RCCAHB1ENR

  *GPIOG_MODER &= ~(1 << 29); // Clear bit 29 of GPIOG_MODER
  *GPIOG_MODER |= (1 << 28); // Set bit 28 of RCCAHB1ENR

  // Per doc, the output is push-pull by default (yay)
  // And we should also set the output speed, but really
  // we don't care (we're doing something super slow)

  long delay = 10000;
  long * GPIOG_PUPDR = (long *)(0x40021800 + 0xC); // Per doc, ref man page 283
  long * GPIOG_ODR = (long *)(0x40021800 + 0x14); // per doc, data register of GPIO G

  while (1) {
    /*
    // Pull-up pin 13
    *GPIOG_PUPDR &= ~(1 << 27);
    *GPIOG_PUPDR |= (1 << 26);
    // Pull-up pin 14
    *GPIOG_PUPDR &= ~(1 << 29);
    *GPIOG_PUPDR |= (1 << 28);
    */
    *GPIOG_ODR = 0x2000;

    sleep(100000);

    *GPIOG_ODR = 0x4000;

    /*
    // Pull-down pin 13
    *GPIOG_PUPDR &= ~(1 << 26);
    *GPIOG_PUPDR |= (1 << 27);
    // Pull-down pin 14
    *GPIOG_PUPDR &= ~(1 << 28);
    *GPIOG_PUPDR |= (1 << 29);
    */

    sleep(100000);
  }
}

void sleep(long delay) {
  for (long i=0; i<delay; i++) {
    // Do nothing!
  }
}
