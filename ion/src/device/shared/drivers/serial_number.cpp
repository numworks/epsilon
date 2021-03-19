#include "serial_number.h"
#include <drivers/config/serial_number.h>
#include "base64.h"
#include <ion.h>

namespace Ion {
namespace Device {
namespace SerialNumber {

const char * get() {
  static char serialNumberBuffer[Length + 1] = {0};
  if (serialNumberBuffer[0] == 0) {
    copy(serialNumberBuffer);
  }
  return serialNumberBuffer;
}

void copy(char * buffer) {
  const unsigned char * rawUniqueID = reinterpret_cast<const unsigned char *>(Config::UniqueDeviceIDAddress);
  Base64::encode(rawUniqueID, 12, buffer);
  buffer[Length] = 0;
}

}
}
}
