#ifndef ION_DEVICE_BENCH_COMMAND_LIST_H
#define ION_DEVICE_BENCH_COMMAND_LIST_H

#include "command_handler.h"

namespace Ion {
namespace Device {
namespace Bench {

class CommandList {
public:
  constexpr CommandList(const CommandHandler * handlers) : m_handlers(handlers) {}
  const CommandHandler * dispatch(const char * command) const;
private:
  const CommandHandler * m_handlers;
};

}
}
}

#endif
