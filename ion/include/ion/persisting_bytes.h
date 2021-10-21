#ifndef ION_PERSISTING_BYTES_H
#define ION_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace PersistingBytes {

void write(uint8_t byte, uint8_t index);
uint8_t read(uint8_t index);
/* Warning : All persisting bytes are expected to be initialized to 0 in
 * shared_kernel_flash.ld which assumes k_numberOfPersistingBytes is 2. */
static constexpr uint8_t k_numberOfPersistingBytes = 2;

}
}

#endif
