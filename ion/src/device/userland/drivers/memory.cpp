#include <ion/memory.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Memory {

void SVC_ATTRIBUTES persistByteSVC(uint8_t * byte) {
  SVC(SVC_MEMORY_PERSIST_BYTE);
}

void PersistByte(uint8_t byte) {
  persistByteSVC(&byte);
}

void SVC_ATTRIBUTES persistedByteSVC(uint8_t * byte) {
  SVC(SVC_MEMORY_READ_PERSISTED_BYTE);
}

uint8_t PersistedByte() {
  uint8_t res;
  persistedByteSVC(&res);
  return res;
}

}
}
