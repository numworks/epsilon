#include <drivers/svcall.h>
#include <ion.h>

namespace Ion {
namespace Device {
namespace SerialNumber {

void SVC_ATTRIBUTES copy(char* buffer) {
  SVC_RETURNING_VOID(SVC_SERIAL_NUMBER_COPY)
}

}  // namespace SerialNumber
}  // namespace Device
}  // namespace Ion
