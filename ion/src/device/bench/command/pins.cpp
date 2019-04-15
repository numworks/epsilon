#include "command.h"
#include <ion.h>
#include <ion/src/device/display.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Pins(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }

  // Put all testable GPIO to pull down and verify they all read 0
  for (const GPIOPin & pinDown : Ion::Device::Display::FSMCPins /* TODO larger group */) {
    pinDown.group().MODER()->setMode(pinDown.pin(), GPIO::MODER::Mode::Input);
    pinDown.group().PUPDR()->setPull(pinDown.pin(), GPIO::PUPDR::Pull::Down);
    Ion::Timing::msleep(100);
    if (pinDown.group().IDR()->get(pinDown.pin())) {
      char response[] = {'P', 'i', 'n', 'D', 'o', 'w', 'n', 0, 0, 0};
      response[7] = '0' + pinDown.group();
      response[8] = '0' + pinDown.pin();
      reply(response);
      return;
    }
  }

  // TODO twice the number of tests...
  // Pull-up GPIOs one at a time and verify it does not impact other GPIOs
  for (const GPIOPin & pinUp : Ion::Device::Display::FSMCPins) {
    pinUp.group().PUPDR()->setPull(pinUp.pin(), GPIO::PUPDR::Pull::Up);
    Ion::Timing::msleep(100);
    if (!(pinUp.group().IDR()->get(pinUp.pin()))) {
      char response[] = {'P', 'i', 'n', 'U', 'p', 0, 0, 0};
      response[5] = '0' + pinUp.group();
      response[6] = '0' + pinUp.pin();
      reply(response);
      return;
    }
    for (const GPIOPin & pinDown : Ion::Device::Display::FSMCPins) {
      if (pinUp != pinDown && pinDown.group().IDR()->get(pinDown.pin())) {
        char response[] = {'P', 'i', 'n', 'U', 'p',
          0, 0, 'P', 'i', 'n',
          'D', 'o', 'w', 'n', 0,
          0, 0};
        response[5] = '0' + pinUp.group();
        response[6] = '0' + pinUp.pin();
        response[14] = '0' + pinDown.group();
        response[15] = '0' + pinDown.pin();
        reply(response);
        return;
      }
    }
    pinUp.group().PUPDR()->setPull(pinUp.pin(), GPIO::PUPDR::Pull::Down);
  }
  reply(sOK);
}

}
}
}
}
