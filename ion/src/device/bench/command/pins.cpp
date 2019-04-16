#include "command.h"
#include <ion.h>
#include <drivers/config/display.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void printPin(Ion::Device::Regs::GPIOPin pin, char * buffer) {
  // WARNING: There should be enough space in buffer for 3 chars.
  assert(pin.group() < 10);
  assert(pin.pin() < 100);
  buffer[0] = '0' + pin.group();
  int tens = pin.pin()/10;
  buffer[1] = '0' + tens;
  buffer[2] = '0' + (pin.pin() - tens * 10);
}

enum class PinType : uint8_t {
  PullUp,
  PullDown,
  OutputUp,
  OutputDown
};

void setPin(Ion::Device::Regs::GPIOPin p, PinType type) {
  if (type == PinType::PullUp || type == PinType::PullDown) {
    p.group().MODER()->setMode(p.pin(), Ion::Device::Regs::GPIO::MODER::Mode::Input);
    p.group().PUPDR()->setPull(p.pin(), type == PinType::PullUp ? Ion::Device::Regs::GPIO::PUPDR::Pull::Up : Ion::Device::Regs::GPIO::PUPDR::Pull::Down);
  } else {
    assert(type == PinType::OutputUp || type == PinType::OutputUp);
    p.group().MODER()->setMode(p.pin(), Ion::Device::Regs::GPIO::MODER::Mode::Output);
    p.group().ODR()->set(p.pin(), type == PinType::OutputUp);
  }
}

void Pins(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  /* We test all pins except:
   * - A0:  BAT_CHRG
   * - A9:  VBUS
   * - B10: LCD_TE
   * - C11: USART_RX
   * - C12: USART_CLK
   * - D8:  USART_TX */

  const Ion::Device::Regs::GPIOPin pins[] = {
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 2),  // KBD_ROW_C
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 3),  // KBD_ROW_D
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 4),  // KBD_ROW_E
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 5),  // KBD_ROW_F
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 6),  // KBD_ROW_G
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 13), // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 14), // OSC32_IN
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 15), // OSC32_OUT
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 0),  // KBD_COL_1
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 1),  // KBD_COL_2
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 2),  // KBD_COL_3
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 3),  // KBD_COL_4
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 1),  // VBAT_SNS
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 2),  // LCD_D4

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 3),  // LCD_D5
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 4),  // LCD_D6
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 5),  // SPI_CLK
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 6),  // SPI_MISO
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 7),  // SPI_MOSI
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 4),  // KBD_COL_5
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 5),  // KBD_COL_6
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 0),  // LED_BLUE
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 1),  // LED_GREEN
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 2),  // QUADSPI_CLK
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 7),  // KBD_ROW_H
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 8),  // KBD_ROW_I
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 9),  // LCD_RESET
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 10), // LCD_D7
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 11), // LCD_D8
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 12), // LCD_D9
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 13), // LCD_D10
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 14), // LCD_D11
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 15), // LCD_D12

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 12), // LCD_D13
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 13), // LCD_EXTC
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 14), // LCD_POW_EN
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 15), // SDIO_CLK
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 9),  // LCD_D14
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 10), // LCD_D15
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 11), // LCD_DAT_INS
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 12), // QUAD_SPI_IO1
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 13), // QUAD_SPI_IO3
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 14), // LCD_D0
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 15), // LCD_D1
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 6),  // LCD_LIGHT
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 7),  // LED_RED
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 8),  // QUADSPI_IO2
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 9),  // QUADSPI_IO0
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 8),  // SDIO_D1
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 10), // USB_ID
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 11), // USB_D-
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 12), // USB_D+
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 13), // SWD_SWDIO

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 14), // SWD_SWCLK
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOA, 15), // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 10), // SDIO_D2
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 0),  // LCD_D2
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 1),  // LCD_D3
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 2),  // SDIO_CMD
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 3),  // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 4),  // LCD_NOE
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 5),  // LCD_NWE
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 6),  // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 7),  // LCD_CSX
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 3),  // SWD_SWO
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 4),  // SDIO_D0
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 5),  // SDIO_D3
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 6),  // QUADSPI_NCS
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 7),  // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 8),  // Connected to nothing
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 9),  // SD_POW_EN
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 0),  // KBD_ROW_A
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 1),  // KBD_ROW_B
  };

  int numberOfPins = sizeof(pins)/sizeof(Ion::Device::Regs::GPIOPin);


  // Put all testable GPIO to pull down and verify they all read 0
  for (const Ion::Device::Regs::GPIOPin & pinDown : pins) {
    setPin(pinDown, PinType::PullDown);
  }

  // Put one GPIO at a time on output 1 and verify it does not impact other GPIOs
  for (int i = 0; i < numberOfPins; i++) {
    const Ion::Device::Regs::GPIOPin & currentPin = pins[i];
    setPin(currentPin, PinType::OutputUp);
    Ion::Timing::msleep(10); //TODO
    for (int j = i+1; j < numberOfPins; j++) {
      const Ion::Device::Regs::GPIOPin & pinDown = pins[j];
      if (pinDown.group().IDR()->get(pinDown.pin())) {
        char response[] = {'A', 'l', 'l', '_', 'p', 'i', 'n', 's', '_', 'd', 'o', 'w', 'n', ':', 'F', 'a', 'i', 'l', '_', 'P', 'i', 'n', 'U', 'p', 0, 0, 0, 'P', 'i', 'n', 'D', 'o', 'w', 'n', 0, 0, 0};
        printPin(currentPin, response + 24);
        printPin(pinDown, response + 34);
        reply(response);
        return;
      }
    }

    // Put the pin on pull up for the next test
    setPin(currentPin, PinType::PullUp);
  }

  /* All pins are in pull up. Put one GPIO at a time on output 0 and verify it
   * does not impact other GPIOs. */

  for (int i = 0; i < numberOfPins - 1; i++) {
    const Ion::Device::Regs::GPIOPin & currentPin = pins[i];
    setPin(currentPin, PinType::OutputDown);
    Ion::Timing::msleep(10); //TODO
    for (int j = i+1; j < numberOfPins; j++) {
      const Ion::Device::Regs::GPIOPin & pinUp = pins[j];
      if (pinUp.group().IDR()->get(pinUp.pin())) {
        char response[] = {'A', 'l', 'l', '_', 'p', 'i', 'n', 's', '_', 'u', 'p', ':', 'F', 'a', 'i', 'l', '_', 'P', 'i', 'n', 'U', 'p', 0, 0, 0, 'P', 'i', 'n', 'D', 'o', 'w', 'n', 0, 0, 0};
        printPin(pinUp, response + 22);
        printPin(currentPin, response + 32);
        reply(response);
        return;
      }
    }
    // Put the pin on pull down so that all pins are not in output up
    setPin(currentPin, PinType::PullDown);
  }

  reply(sOK);
}

}
}
}
}
