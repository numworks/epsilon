#include "command.h"

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

void Ping(const char * input) {
  if (input != nullptr) {
    reply(sSyntaxError);
    return;
  }
  reply("PONG");
}

}
}
}
}
