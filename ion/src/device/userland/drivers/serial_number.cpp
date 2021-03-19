#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void SVC_ATTRIBUTES serialNumberSVC(const char ** buffer) {
  SVC(SVC_SERIAL_NUMBER);
}

const char * serialNumber() {
  const char * res;
  serialNumberSVC(&res);
  return res;
}

}

namespace Ion {
namespace Device {
namespace SerialNumber {

void SVC_ATTRIBUTES copySVC(char * buffer) {
  SVC(SVC_SERIAL_NUMBER_COPY);
}

void copy(char * buffer) {
  copySVC(buffer);
}

}
}
}
