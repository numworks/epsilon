#include <registers.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>

/* This code sends data over SPI
 * We're using SPI4, in this pinout
 *
 * PE11 - SPI4_NSS - Yellow
 * PE12 - SPI4_SCK - Green
 * PE13 - SPI4_MISO - Red
 * PE14 - SPI4_MOSI - Blue
 */

void SpiSend(void * pvParameters) {
  uint16_t value = 0;
  while(1) {
    SPI_SR_t * spi_status = SPI_SR(SPI4);
    if (spi_status->BSY == 0) {
      uint16_t * SPI4_DR = (uint16_t *)(0x4001340C);
      *SPI4_DR = value++;
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

int main(int argc, char * argv[]) {
  // We'll use GPIO pins E11-E14 to emit SPI data
  // GPIO are grouped by letter, and GPIO "G" live on the "AHB1" bus (as all GPIOs BTW)
  // (this is documented in the STM32F4 reference mnual, page 65)

  // Step 1 : Enable clock in RCC_AHBxENR
  RCC_AHB1ENR->GPIOEEN = 1;

  // Step 2 : Configure the GPIO pin to "Alternate function number 5"
  // This means "SPI4 on pins E11-14", cf STM32F249 p78
  GPIO_MODER(GPIOE)->MODER11 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOE)->MODER12 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOE)->MODER13 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOE)->MODER14 = GPIO_MODE_ALTERNATE_FUNCTION;

  // We're interested in pins 11-14, which are in the 8-16 range, so we
  // have to deal with the "high" version of the AF register
  GPIO_AFRH(GPIOE)->AFRH11 = GPIO_AF_AF5;
  GPIO_AFRH(GPIOE)->AFRH12 = GPIO_AF_AF5;
  GPIO_AFRH(GPIOE)->AFRH13 = GPIO_AF_AF5;
  GPIO_AFRH(GPIOE)->AFRH14 = GPIO_AF_AF5;

  // Enable the SPI4 clock
  RCC_APB2ENR->SPI4EN = 1;

  // Configure the SPI port
  /*
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    */
  SPI_CR1_t * SPI4_CR1 = SPI_CR1(SPI4);
  /*
  SPI4_CR1->BIDIMODE = 0;
  SPI4_CR1->BIDIOE = 1;
  SPI4_CR1->MSTR = 1;
  SPI4_CR1->DFF = SPI_DFF_8_BITS;
  SPI4_CR1->CPOL = 0;
  SPI4_CR1->BR = SPI_BR_DIV_256;
  SPI4_CR1->SSM = 1;
  SPI4_CR1->SSI = 1;
  SPI4_CR1->SPE = 1;
  */

  /*
   11 0 0 1
   0 1 1 0 1 111
   100 */

  /*
   * 1100
   * 1011
   * 0111
   * 1100
   */
  *(uint16_t *)SPI4_CR1 = 0xCB7C;

  SPI_CR1_t output = *SPI4_CR1;

  BaseType_t success = xTaskCreate(SpiSend,
      "SpiSnd",
      100, // Stack size
      NULL, // Parameters
      2,
      NULL);

  vTaskStartScheduler();

  while (1) {
    // We should never get here
  }
}
