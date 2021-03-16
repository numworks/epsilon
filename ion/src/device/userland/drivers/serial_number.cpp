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
