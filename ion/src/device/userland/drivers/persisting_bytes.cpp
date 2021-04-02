#include <ion/persisting_bytes.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace PersistingBytes {

void SVC_ATTRIBUTES writeSVC(uint8_t * byte) {
  SVC(SVC_PERSISTING_BYTES_WRITE);
}

void write(uint8_t byte) {
  writeSVC(&byte);
}

void SVC_ATTRIBUTES readSVC(uint8_t * byte) {
  SVC(SVC_PERSISTING_BYTES_READ);
}

uint8_t __attribute__((externally_visible)) read() {
  SVC(SVC_PERSISTING_BYTES_READ);
}

}
}
