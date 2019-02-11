#include "serial_number.h"
#include <drivers/config/serial_number.h>
#include "base64.h"

namespace Ion {

using namespace Ion::Device::SerialNumber;

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
  Base64::encode((const unsigned char *)RawUniqueIDAddress, 12, buffer);
  buffer[Length] = 0;
}

}
}
}
