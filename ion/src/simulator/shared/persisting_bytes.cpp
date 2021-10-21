#include <ion/persisting_bytes.h>
#include <assert.h>

namespace Ion {
namespace PersistingBytes {

uint8_t sPersistedBytes[k_numberOfPersistingBytes] = {0, 0};
static_assert(k_numberOfPersistingBytes == 2, "sPersistedBytes initial values must be updated here.");

void write(uint8_t byte, uint8_t index) {
  assert(index < k_numberOfPersistingBytes);
  sPersistedBytes[index] = byte;
}

uint8_t read(uint8_t index) {
  assert(index < k_numberOfPersistingBytes);
  return sPersistedBytes[index];
}

}
}
