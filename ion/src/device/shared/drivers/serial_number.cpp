#include "serial_number.h"
#include "board.h"
#include <drivers/config/serial_number.h>
#include "base64.h"

namespace Ion {

using namespace Device::SerialNumber;

const char * serialNumber() {
  static char serialNumber[Length + 1] = {0};
  if (serialNumber[0] == 0) {
    copy(serialNumber);
  }
  return serialNumber;
}

}

namespace Ion {
namespace Device {
namespace SerialNumber {

void copy(char * buffer) {
  const unsigned char * rawUniqueID = reinterpret_cast<const unsigned char *>(Config::UniqueDeviceIDAddress);
  Base64::encode(rawUniqueID, 12, buffer);
  buffer[Length] = 0;
}

}
}
}
