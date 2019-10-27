#include "command.h"
#include <ion.h>
#include <poincare/print_int.h>
#include <drivers/cache.h>
#include <cmath>

namespace Ion {
namespace Device {
namespace Bench {
namespace Command {

// Input must be either "INTERNAL_size" or "EXTERNAL_size"
void CRC(const char * input) {
  bool internal = true;
  int suffixSize = 9;
  if (strncmp(input, "INTERNAL_", suffixSize) == 0) {
    internal = true;
  } else if (strncmp(input, "EXTERNAL_", suffixSize) == 0) {
    internal = false;
  } else {
    reply(sSyntaxError);
    return;
  }

  int lengthStart = suffixSize;
  int lengthEnd = lengthStart;
  int lengthSizeLimit = 15;

  for (int i = 0; i < lengthSizeLimit; i++) {
    char c = input[lengthStart+i] ;
    lengthEnd++;
    if (c == 0) {
      break;
    }
    if (c < '0' || c > '9') {
      reply(sSyntaxError);
      return;
    }
  }
  if (lengthEnd - lengthStart >= lengthSizeLimit) {
    reply(sSyntaxError);
    return;
  }

  uint32_t length = numberBase10(input + lengthStart, lengthEnd - lengthStart);

  // Disable the cache to make many cache accesses
  Ion::Device::Cache::disable();

  uint32_t crc = Ion::crc32Byte(reinterpret_cast<const uint8_t *>(internal ? 0x08000000 : 0x90000000), length);

  Ion::Device::Cache::enable();

  constexpr int bufferSize = 4+10+1; // crc is a uint32_t so 10 digits long.
  char buffer[bufferSize] = {'C', 'R', 'C', '='};
  for (int i  = 4; i < bufferSize; i++) {
    buffer[i] = 0;
  }
  Poincare::PrintInt::Left(crc, buffer+4, bufferSize - 4 - 1);
  reply(buffer);
}

}
}
}
}
