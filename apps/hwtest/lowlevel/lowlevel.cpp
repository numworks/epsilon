#include <stddef.h>
#include <ion.h>

typedef void (*CommandFunction)(const char * input);

void command_ping(const char * input);
void command_mcu_serial(const char * input);

class CommandHandler {
public:
  constexpr CommandHandler(const char * name, CommandFunction function) :
    m_name(name), m_function(function) {}
  bool valid() const;
  bool handle(const char * command) const;
private:
  bool matches(const char * command) const;
  const char * m_name;
  CommandFunction m_function;
};

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

class CommandList {
public:
  constexpr CommandList(const CommandHandler * handlers) : m_handlers(handlers) {}
  void dispatch(const char * command) const;
private:
  const CommandHandler * m_handlers;
};

void CommandList::dispatch(const char * command) const {
  const CommandHandler * handler = m_handlers;
  while (handler->valid()) {
    if (handler->handle(command)) {
      return;
    }
    handler++;
  }
  Ion::Console::writeLine("NOT_FOUND");
}



void command_ping(const char * input) {
  Ion::Console::writeLine("PONG");
}

void command_mcu_serial(const char * input) {
  Ion::Console::writeLine("UNKNOWN");
}


static inline int8_t hexChar(char c) {
  if (c >= '0' && c <= '9') {
    return (c - '0');
  }
  if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 0xA;
  }
  return -1;
}
static inline bool isHex(char c) { return hexChar(c) >= 0; }
static inline uint32_t hexNumber(const char * s) {
  uint32_t result = 0;
  while (*s != NULL) {
    result = (result << 4) | hexChar(*s++);
  }
  return result;
}

void command_led(const char * input) {
  // Input must be of the form "0xAABBCC"
  if (input == nullptr || input[0] != '0' || input[1] != 'x' || !isHex(input[2]) ||!isHex(input[3]) || !isHex(input[4]) || !isHex(input[5]) || !isHex(input[6]) || !isHex(input[7]) || input[8] != NULL) {
    Ion::Console::writeLine("SYNTAX_ERROR");
    return;
  }
  uint32_t hexColor = hexNumber(input+2);
  KDColor ledColor = KDColor::RGB24(hexColor);
  Ion::LED::setColor(ledColor);
  Ion::Console::writeLine("OK");
}

constexpr CommandHandler handles[] = {
  CommandHandler("PING", command_ping),
  CommandHandler("MCU_SERIAL", command_mcu_serial),
  CommandHandler("LED", command_led),
  CommandHandler(nullptr, nullptr)
};
constexpr const CommandList sCommandList = CommandList(handles);

constexpr int kMaxCommandLength = 255;

void ion_app() {
  char command[kMaxCommandLength];
  while (true) {
    Ion::Console::readLine(command, kMaxCommandLength);
    sCommandList.dispatch(command);
  }
}

