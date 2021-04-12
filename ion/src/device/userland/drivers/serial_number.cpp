#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Device {
namespace SerialNumber {

void SVC_ATTRIBUTES copy(char * buffer) {
  SVC(SVC_SERIAL_NUMBER_COPY);
}

}
}
}

namespace Ion {

const char * SVC_ATTRIBUTES serialNumber() {
  static char serialNumberBuffer[k_serialNumberLength + 1] = {0};
  if (serialNumberBuffer[0] == 0) {
    Device::SerialNumber::copy(serialNumberBuffer);
  }
  return serialNumberBuffer;
}

}
