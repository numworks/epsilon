#include <ion/persisting_bytes.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace PersistingBytes {

void SVC_ATTRIBUTES write(uint8_t byte) {
  SVC_RETURNING_VOID(SVC_PERSISTING_BYTES_WRITE)
}

uint8_t SVC_ATTRIBUTES read() {
  SVC_RETURNING_R0(SVC_PERSISTING_BYTES_READ, uint8_t)
}

}
}
