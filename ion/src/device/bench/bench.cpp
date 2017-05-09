#include "bench.h"
#include <ion.h>
#include "command_list.h"

namespace Ion {
namespace Device {
namespace Bench {

constexpr CommandHandler handles[] = {
  CommandHandler("ADC", Command::ADC),
  CommandHandler("BACKLIGHT", Command::Backlight),
  CommandHandler("CHARGE", Command::Charge),
  CommandHandler("DISPLAY", Command::Display),
  CommandHandler("EXIT", Command::Exit),
  CommandHandler("KEYBOARD", Command::Keyboard),
  CommandHandler("LED", Command::LED),
  CommandHandler("MCU_SERIAL", Command::MCUSerial),
  CommandHandler("PING", Command::Ping),
  CommandHandler("SUSPEND", Command::Suspend),
  CommandHandler(nullptr, nullptr)
};

constexpr const CommandList sCommandList = CommandList(handles);

constexpr int kMaxCommandLength = 255;

void run() {
  char command[kMaxCommandLength];
  while (true) {
    Ion::Console::readLine(command, kMaxCommandLength);
    const CommandHandler * ch = sCommandList.dispatch(command);
    if (ch != nullptr && ch->function() == Command::Exit) {
      break;
    }
  }
}

}
}
}
