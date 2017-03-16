#include "command.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

const char * const sOK = "OK";
const char * const sKO = "KO";
const char * const sSyntaxError = "SYNTAX_ERROR";
const char * const sON = "ON";
const char * const sOFF = "OFF";

void reply(const char * s) {
  Console::writeLine(s);
}

int8_t hexChar(char c) {
  if (c >= '0' && c <= '9') {
    return (c - '0');
  }
  if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 0xA;
  }
  return -1;
}

bool isHex(char c) {
  return hexChar(c) >= 0;
}

uint32_t hexNumber(const char * s) {
  uint32_t result = 0;
  while (*s != NULL) {
    result = (result << 4) | hexChar(*s++);
  }
  return result;
}


}
}
}
}
