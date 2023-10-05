#include <drivers/persisting_bytes.h>
#include <drivers/svcall.h>

namespace Ion {
namespace PersistingBytes {

uint8_t* SVC_ATTRIBUTES bufferStart() {
  SVC_RETURNING_R0(SVC_PERSISTING_BYTES_BUFFER_START, uint8_t*);
}

uint8_t* SVC_ATTRIBUTES bufferEnd() {
  SVC_RETURNING_R0(SVC_PERSISTING_BYTES_BUFFER_END, uint8_t*);
}

}  // namespace PersistingBytes
}  // namespace Ion
