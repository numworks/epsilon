#include <ion/persisting_bytes.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace PersistingBytes {

void SVC_ATTRIBUTES write(PersistingBytesInt value) {
  SVC_RETURNING_VOID(SVC_PERSISTING_BYTES_WRITE)
}

PersistingBytesInt SVC_ATTRIBUTES read() {
  SVC_RETURNING_R0(SVC_PERSISTING_BYTES_READ, PersistingBytesInt)
}

}
}
