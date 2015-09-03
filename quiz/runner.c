#include <registers.h>

void assert(int condition);

void low_level_sleep(int iterations) {
  for (int i=0;i<iterations;i++) {
  }
}

void debugger() {
  while(1) {
  }
}

void halt(int success) {
  RCC_AHB1ENR->GPIOGEN = 1;
  if (success) {
    GPIO_MODER(GPIOG)->MODER13 = GPIO_MODE_OUTPUT;
    for (int i=0;i<5; i++) {
      GPIO_ODR(GPIOG)->ODR13 = 0;
      low_level_sleep(50000);
      GPIO_ODR(GPIOG)->ODR13 = 1;
      low_level_sleep(50000);
    }
  } else {
    GPIO_MODER(GPIOG)->MODER14 = GPIO_MODE_OUTPUT;
    for (int i=0;i<5; i++) {
      GPIO_ODR(GPIOG)->ODR14 = 0;
      low_level_sleep(50000);
      GPIO_ODR(GPIOG)->ODR14 = 1;
      low_level_sleep(50000);
    }
  }
  debugger();
}

void assert(int condition) {
  if (!condition) {
    halt(0);
  }
}

int main(int argc, char * argv[]) {
  // Run assertions
  halt(1);
}
