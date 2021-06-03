#include <ion/persisting_bytes.h>

namespace Ion {
namespace PersistingBytes {

uint8_t sPersistedByte = 0;

void write(uint8_t byte) {
  sPersistedByte = byte;
}

uint8_t read() {
  return sPersistedByte;
}

}
}
