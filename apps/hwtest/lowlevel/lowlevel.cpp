#include <stddef.h>
#include <ion.h>

#include "../../../ion/src/device/regs/regs.h"

// PB3 = UART1_RX = SWO
// PB6 = UART1_TX = QuadSPI NCS

// On Rpi, from top to bottom
// 6 = GND
// 8 = TX
// 10 = RX

constexpr USART uartPort = USART(1);

void init_uart_port() {
  RCC.APB2ENR()->setUSART1EN(true);

  GPIOB.MODER()->setMode(3, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(6, GPIO::MODER::Mode::AlternateFunction);

  GPIOB.AFR()->setAlternateFunction(3, GPIO::AFR::AlternateFunction::AF7);
  GPIOB.AFR()->setAlternateFunction(6, GPIO::AFR::AlternateFunction::AF7);

  uartPort.CR1()->setUE(true);
  uartPort.CR1()->setTE(true);
  uartPort.CR1()->setRE(true);

  // Set the Baud rate
  // base clock = 16 MHz
  // Baud rate = fAPB2/(16*USARTDIV)
  // USARTDIV = 104.16667
  //
  // DIV_Fraction = 16*0.16666667
  //  = 2.666667 -> 3
  // DIV_Mantissa = 104 = 0x68
  // USART_BRR = 0x683
  uartPort.BRR()->setDIV_FRAC(3);
  uartPort.BRR()->setDIV_MANTISSA(104);
}

char uart_read_char() {
  while (uartPort.SR()->getRXNE() == 0) {
  }
  return (char)uartPort.DR()->get();
}

void uart_write_char(char c) {
  while (uartPort.SR()->getTXE() == 0) {
  }
  uartPort.DR()->set(c);
}

bool readLine(char * buffer, int bufferSize) {
  char * endBuffer = buffer + bufferSize - 1;
  while (true) {
    *buffer = uart_read_char();
    if (*buffer == NULL) {
      /* Time for a comment :
       * - Building DEBUG=0 might make the device fast enough
       * - Reading on the HOST... ALSO sends data to the device !!
       *   So we were being overflowed with zeroes here... */
      continue;
    }
    if (*buffer == '\r') {
      break;
    }
    buffer++;
    if (buffer == endBuffer) {
      return false;
    }
  }
  *buffer = 0;
  return true;
}

void reply(const char * buffer) {
  while (*buffer != NULL) {
    uart_write_char(*buffer);
    buffer++;
  }
  uart_write_char('\r');
  uart_write_char('\n');
}

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
  reply("NOT_FOUND");
}



void command_ping(const char * input) {
  reply("PONG");
}

void command_mcu_serial(const char * input) {
  reply("UNKNOWN");
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
    reply("SYNTAX_ERROR");
    return;
  }
  uint32_t hexColor = hexNumber(input+2);
  KDColor ledColor = KDColor::RGB24(hexColor);
  Ion::LED::setColor(ledColor);
  reply("OK");
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
  init_uart_port();
  char command[kMaxCommandLength];
  while (true) {
    if (readLine(command, kMaxCommandLength)) {
      sCommandList.dispatch(command);
    }
  }
}

