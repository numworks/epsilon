#ifndef ION_DEVICE_BENCH_COMMAND_HANDLER_H
#define ION_DEVICE_BENCH_COMMAND_HANDLER_H

#include "command/command.h"

namespace Ion {
namespace Device {
namespace Bench {

class CommandHandler {
public:
  constexpr CommandHandler(const char * name, Command::Function function) :
    m_name(name), m_function(function) {}
  bool valid() const;
  bool handle(const char * command) const;
  Command::Function function() const { return m_function; }
private:
  bool matches(const char * command) const;
  const char * m_name;
  Command::Function m_function;
};

}
}
}

#endif
