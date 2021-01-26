#include <ion.h>
#include <userland/drivers/svcall.h>

namespace Ion {

void SVC_ATTRIBUTES serialNumberSVC(char * buffer) {
  SVC(SVC_SERIAL_NUMBER);
}

void serialNumber(char * buffer) {
  serialNumberSVC(buffer);
}

}
