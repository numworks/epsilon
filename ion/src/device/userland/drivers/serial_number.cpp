#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

const char * SVC_ATTRIBUTES serialNumber() {
  SVC(SVC_SERIAL_NUMBER);
}

}

namespace Ion {
namespace Device {
namespace SerialNumber {

void SVC_ATTRIBUTES copy(char * buffer) {
  SVC(SVC_SERIAL_NUMBER_COPY);
}

}
}
}
