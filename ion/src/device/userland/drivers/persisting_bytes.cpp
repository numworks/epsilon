#include <ion/persisting_bytes.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace PersistingBytes {

void SVC_ATTRIBUTES write(uint8_t byte, uint8_t index) {
  SVC_RETURNING_VOID(SVC_PERSISTING_BYTES_WRITE)
}

uint8_t SVC_ATTRIBUTES read(uint8_t index) {
  SVC_RETURNING_R0(SVC_PERSISTING_BYTES_READ, uint8_t)
}

}
}
