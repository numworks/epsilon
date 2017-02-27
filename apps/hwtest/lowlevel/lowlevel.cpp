#include <stddef.h>

#include "../../../ion/src/device/regs/regs.h"

// SDIOCK -> SPI2_MOSI
// SDIO_D2 -> SPI3_CLK == PC10
// SDIO_D0 -> SPI3_MISO = PB4
// SDIO_D3 -> SPI3_MOSI = PB5
//
//

// On SD card, from left to right
// D1 - D0 - .... - D3 - D2
// X - MISO - .... - MOSI - CLK

// On Rpi, from top to bottom
// MOSI 19
// MISO 21
// CLK 23
// GND 25
constexpr SPI spiPort = SPI(3);

void init_spi_port() {
  RCC.APB1ENR()->setSPI3EN(true);

  GPIOB.MODER()->setMode(4, GPIO::MODER::Mode::AlternateFunction);
  GPIOB.MODER()->setMode(5, GPIO::MODER::Mode::AlternateFunction);
  GPIOC.MODER()->setMode(10, GPIO::MODER::Mode::AlternateFunction);

  GPIOB.AFR()->setAlternateFunction(4, GPIO::AFR::AlternateFunction::AF6);
  GPIOB.AFR()->setAlternateFunction(5, GPIO::AFR::AlternateFunction::AF6);
  GPIOC.AFR()->setAlternateFunction(10, GPIO::AFR::AlternateFunction::AF6);

  spiPort.CR1()->setSPE(true);

  // We don't use a CS wire
  spiPort.CR1()->setSSM(true);
  spiPort.CR1()->setSSI(0);
}

char spi_read_char() {
  while (spiPort.SR()->getRXNE() == 0) {
  }
  return (char)spiPort.DR()->get();
}

void spi_write_char(char c) {
  spiPort.DR()->set(c);
  while (spiPort.SR()->getTXE() == 0) {
  }
}

bool readLine(char * buffer, int bufferSize) {
  char * endBuffer = buffer + bufferSize - 1;
  while (true) {
    *buffer = spi_read_char();
    if (*buffer == NULL) {
      /* Time for a comment :
       * - Building DEBUG=0 might make the device fast enough
       * - Reading on the HOST... ALSO sends data to the device !!
       *   So we were being overflowed with zeroes here... */
      continue;
    }
    if (*buffer == '\n') {
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
    spi_write_char(*buffer);
    buffer++;
  }
  spi_write_char('\n');
  spi_write_char(0);
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
    m_function(command);
    return true;
  }
  return false;
}

bool CommandHandler::matches(const char * command) const {
  const char * c = command;
  const char * n = m_name;
  while (true) {
    if (*c != *n) {
      return false;
    }
    if (*n == NULL) {
      if (*c == NULL || *c == '=') {
        return true;
      }
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

constexpr CommandHandler handles[] = {
  CommandHandler("PING", command_ping),
  CommandHandler("MCU_SERIAL", command_mcu_serial),
  CommandHandler(nullptr, nullptr)
};

constexpr const CommandList sCommandList = CommandList(handles);

void command_ping(const char * input) {
  reply("PONG");
}

void command_mcu_serial(const char * input) {
  reply("UNKNOWN");
}

constexpr int kMaxCommandLength = 255;

void ion_app() {
  init_spi_port();
  char command[kMaxCommandLength];
  while (true) {
    if (readLine(command, kMaxCommandLength)) {
      sCommandList.dispatch(command);
    }
  }
}

