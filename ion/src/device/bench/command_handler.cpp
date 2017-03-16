#include "command_handler.h"
#include <string.h>

namespace Ion {
namespace Device {
namespace Bench {

bool CommandHandler::valid() const {
  return (m_name != nullptr && m_function != nullptr);
}

bool CommandHandler::handle(const char * command) const {
  if (matches(command)) {
    size_t nameLength = strlen(m_name);
    if (command[nameLength] == '=') {
      m_function(command+nameLength+1); // Skip the "Equal character"
    } else {
      m_function(nullptr);
    }
    return true;
  }
  return false;
}

bool CommandHandler::matches(const char * command) const {
  const char * c = command;
  const char * n = m_name;
  while (true) {
    if (*n == NULL) {
      if (*c == NULL || *c == '=') {
        return true;
      }
    }
    if (*c != *n) {
      return false;
    }
    c++;
    n++;
  }
}

}
}
}
