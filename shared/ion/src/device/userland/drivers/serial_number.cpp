#include <ion.h>
#include <shared/drivers/serial_number.h>

namespace Ion {

const char* serialNumber() {
  static char serialNumberBuffer[k_serialNumberLength + 1] = {0};
  if (serialNumberBuffer[0] == 0) {
    Device::SerialNumber::copy(serialNumberBuffer);
  }
  return serialNumberBuffer;
}

}  // namespace Ion
