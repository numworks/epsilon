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

const char * pcbVersion() {
  constexpr int pcbVersionLength = 5; // xx.yy
  static char pcbVer[pcbVersionLength] = {'\0'};
  if (pcbVer[0] == '\0') {
    Device::Board::PCBVersion ver = Device::Board::pcbVersion();
    assert(ver < 10000);
    for (int i = pcbVersionLength - 1; i >= 0; i--) {
      if (i == 2) {
        pcbVer[i] = '.';
      } else {
        pcbVer[i] = '0' + ver % 10;
        ver /= 10;
      }
    }
  }
  return pcbVer;
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
