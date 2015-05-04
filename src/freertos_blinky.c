#include <registers.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>

void BlinkGreenLed(void * pvParameters) {
  while(1) {
    vTaskDelay(100);
    GPIO_ODR(GPIOG)->ODR13 = 0;
    vTaskDelay(100);
    GPIO_ODR(GPIOG)->ODR13 = 1;
  }
}

void BlinkRedLed(void * pvParameters) {
  int delay = 10;
  while (1) {
    vTaskDelay(delay);
    GPIO_ODR(GPIOG)->ODR14 = 1;
    vTaskDelay(delay);
    GPIO_ODR(GPIOG)->ODR14 = 0;
    delay++;
  }
}

int main(int argc, char * argv[]) {
    // We want to blink LEDs connected to GPIO pin G13 and G14
  // (this is documented in our board's PDF)
  //
  // GPIO are grouped by letter, and GPIO "G" live on the "AHB1" bus
  // (this is documented in the STM32F4 reference mnual, page 65)

  // Step 1 : Enable clock in RCC_AHBxENR
  RCC_AHB1ENR->GPIOGEN = 1;


  // Step 2 : Configure the GPIO pin to "general purpose output
  GPIO_MODER(GPIOG)->MODER13 = GPIO_MODE_OUTPUT;
  GPIO_MODER(GPIOG)->MODER14 = GPIO_MODE_OUTPUT;


  BaseType_t success = xTaskCreate(BlinkGreenLed,
      "BlkGrn",
      100, // Stack size
      NULL, // Parameters
      2,
      NULL);

  xTaskCreate(BlinkRedLed, "BlkRed", 100, NULL, 2, NULL);

  vTaskStartScheduler();

  while (1) {
    // We should never get here
  }
}
