#include "command_list.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {

void CommandList::dispatch(const char * command) const {
  const CommandHandler * handler = m_handlers;
  while (handler->valid()) {
    if (handler->handle(command)) {
      return;
    }
    handler++;
  }
  Console::writeLine("NOT_FOUND");
}

}
}
}
