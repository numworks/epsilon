#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <semphr.h>

#include <platform/stm32f429/registers.h>
#include <platform/stm32f429/spi.h>

/* This code sends data to the onboard LCD over SPI
 *
 * The LCD has two interfaces, SPI and direct RGB.
 * We'll only use SPI in this one.
 *
 * The Discovery board has the IM[0-3] pins of the LCD controller
 * connected to low, high, high, low = 0b0110. (UM1670 p. 33).
 * This tells the LCD controller to expect data this way:
 * "4 wire, 8-bit serial, SDA = In/out"
 * See ILI9341 p. 63
 *
 * Now how are the LCD pins connected to the MCU ones?
 * The Discovery board doc says this:
 * MCU pin -  LCD-SPI
 *   NRST  -  Reset
 *   PC2   - CSX // Chip select for LCD
 *    PD13 - DCX // Data/Command register
 *     PF7 - SCL // SPI Clock
 *     PF9 - SDI/SDO // MOSI
 *
 * See UM1670 p. 19 to 24. */

/* We'll need to figure out which GPIO pins this maps to, and in which
 * Alternate Function mode. STM32F429 datasheet p78-79 says:
 * PF6 in AF5 = SPI5_NSS
 * PF7 in AF5 = SPI5_SCK
 * PF8 in AF5 = SPI5_MISO
 * PF9 in AF5 = SPI5_MOSI
 */

/* In case we want to monitor this, here's a color-mapping
 *
 * PF6 - SPI5_NSS - Yellow
 * PF7 - SPI5_SCK - Green
 * PF8 - SPI5_MISO - Red
 * PF9 - SPI5_MOSI - Blue
 */

void SpiSend(void * pvParameters) {
  uint16_t value = 0;
  while(1) {
    SPI_SR_t * spi_status = SPI_SR(SPI5);
    SPI_DR_t * spi_data_register = SPI_DR(SPI5);
    if (spi_status->BSY == 0) {
      *spi_data_register = value++;
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}
/*
struct spi_port {
  // Private data:
  .config = {
  },
  .state = {
  }
};

// Public API, private impl.
spi_init();
spi_write();
*/
int main(int argc, char * argv[]) {

  spi_port_t my_spi_port = {
    .config = {
      .controlRegister1 = {
        .BIDIMODE = 1,
        .BIDIOE = 1,
        .MSTR = 1,
        .DFF = SPI_DFF_16_BITS,
        .CPOL = 0,
        .BR = SPI_BR_DIV_256,
        .SSM = 1,
        .SSI = 1,
        .SPE = 1
      }
    }
  };
  spi_init(&my_spi_port);

  /*
  // Code we'd like to write:

  spi_port my_spi_port;
  spi_init(&my_spi_port);

  ili9431 lcd_panel = {
    .port = my_spi_port
  };

  ili9431_init(&lcd_panel);
  lcd_panel.clear();
  lcd_panel.setGammaCurve();

  char * fb = lcd_panel.framebuffer;

  for (int i=0;i<100;i++) {
    *fb[i] = 1;
  }

*/



  // We'll use GPIO pins F6-F9 to emit SPI data
  // GPIO are grouped by letter. All GPIO groups live on the "AHB1" bus.
  // (this is documented in the STM32F4 reference mnual, page 65)

  // Step 1 : Enable clock in RCC_AHBxENR
  RCC_AHB1ENR->GPIOFEN = 1;

  // Step 2 : Set the GPIO pin C2 as output

  RCC_AHB1ENR->GPIOCEN = 1;
  GPIO_MODER(GPIOC)->MODER2 = GPIO_MODE_OUTPUT;

  RCC_AHB1ENR->GPIOCEN = 1;
  GPIO_MODER(GPIOC)->MODER2 = GPIO_MODE_OUTPUT;
  // From now on, we'll control pin C2 with
  // GPIO_ODR(GPIOC)->ODR2 = desiredValue;

  // Step 2 : Configure the GPIO pin to "Alternate function number 5"
  // This means "SPI5 on pins F6-F9", cf STM32F249 p78
  GPIO_MODER(GPIOF)->MODER6 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOF)->MODER7 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOF)->MODER8 = GPIO_MODE_ALTERNATE_FUNCTION;
  GPIO_MODER(GPIOF)->MODER9 = GPIO_MODE_ALTERNATE_FUNCTION;

  // We're interested in pins 11-14, which are in the 8-16 range, so we
  // have to deal with the "high" version of the AF register
  GPIO_AFRL(GPIOF)->AFRL6 = GPIO_AF_AF5;
  GPIO_AFRL(GPIOF)->AFRL7 = GPIO_AF_AF5;
  GPIO_AFRH(GPIOF)->AFRH8 = GPIO_AF_AF5;
  GPIO_AFRH(GPIOF)->AFRH9 = GPIO_AF_AF5;

  // Enable the SPI5 clock
  RCC_APB2ENR->SPI5EN = 1;

  // Configure the SPI port
  // Using a C99 compound litteral. C99 guarantees all non-set values are zero
  *SPI_CR1(SPI5) = (SPI_CR1_t){
    .BIDIMODE = 1,
    .BIDIOE = 1,
    .MSTR = 1,
    .DFF = SPI_DFF_16_BITS,
    .CPOL = 0,
    .BR = SPI_BR_DIV_256,
    .SSM = 1,
    .SSI = 1,
    .SPE = 1
  };

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
