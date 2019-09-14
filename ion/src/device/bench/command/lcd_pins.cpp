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
  buffer[0] = 'A' + pin.group();
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

void LCDPins(const char * input) {
  if (input == nullptr) {
    reply(sSyntaxError);
    return;
  }
  bool checkPowPin = true;
  if (strcmp(input, "WithoutPowPin") == 0) {
    checkPowPin = false;
  } else if (strcmp(input, "WithPowPin") != 0) {
    reply(sSyntaxError);
    return;
  }

  /* If a screen is connected, we do not test checkPowPin: others pins might be
   * affected if we power up the screen. */
  const Ion::Device::Regs::GPIOPin LCDpins[] = {
    /* Put TE first. If it is after RESET, there is a bug:
     * "All_pins_up:Fail_PinUpB11PinDownE01".
     * TODO: investigate... */
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOB, 11), // LCD_TE

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 1),  // LCD_RESET
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 7),  // LCD_D4
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 8),  // LCD_D5
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 9),  // LCD_D6
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 10), // LCD_D7
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 11), // LCD_D8
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 12), // LCD_D9
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 13), // LCD_D10
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 14), // LCD_D11
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOE, 15), // LCD_D12

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 0),  // LCD_D2
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 1),  // LCD_D3
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 4),  // LCD_NOE
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 5),  // LCD_NWE
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 6),  // LCD_EXTC
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 7),  // LCD_CSX
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 8),  // LCD_D13
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 9),  // LCD_D14
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 10), // LCD_D15
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 11), // LCD_DAT_INS
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 14), // LCD_D0
    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOD, 15), // LCD_D1

    Ion::Device::Regs::GPIOPin(Ion::Device::Regs::GPIOC, 8),  // LCD_POW_EN    // Must be the last Pin
  };

  int numberOfPins = sizeof(LCDpins)/sizeof(Ion::Device::Regs::GPIOPin) - (checkPowPin ? 0 : 1);

  // Put all testable GPIO to pull down
  for (int i = 0; i < numberOfPins; i++) {
    const Ion::Device::Regs::GPIOPin & currentPin = LCDpins[i];
    setPin(currentPin, PinType::PullDown);
  }

  // Put one GPIO at a time on output 1 and verify it does not impact other GPIOs
  for (int i = 0; i < numberOfPins; i++) {
    const Ion::Device::Regs::GPIOPin & currentPin = LCDpins[i];
    setPin(currentPin, PinType::OutputUp);
    for (int j = i+1; j < numberOfPins; j++) {
      const Ion::Device::Regs::GPIOPin & pinDown = LCDpins[j];
      if (pinDown.group().IDR()->get(pinDown.pin())) {
        char response[] = {'A', 'l', 'l', '_', 'p', 'i', 'n', 's', '_', 'd', 'o', 'w', 'n', ':', 'F', 'a', 'i', 'l', '_', 'P', 'i', 'n', 'U', 'p', 0, 0, 0, 'P', 'i', 'n', 'D', 'o', 'w', 'n', 0, 0, 0, 0};
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
    const Ion::Device::Regs::GPIOPin & currentPin = LCDpins[i];
    setPin(currentPin, PinType::OutputDown);
    for (int j = i+1; j < numberOfPins; j++) {
      const Ion::Device::Regs::GPIOPin & pinUp = LCDpins[j];
      if (!pinUp.group().IDR()->get(pinUp.pin())) {
        char response[] = {'A', 'l', 'l', '_', 'p', 'i', 'n', 's', '_', 'u', 'p', ':', 'F', 'a', 'i', 'l', '_', 'P', 'i', 'n', 'U', 'p', 0, 0, 0, 'P', 'i', 'n', 'D', 'o', 'w', 'n', 0, 0, 0, 0};
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
