#ifndef ION_PERSISTING_BYTES_H
#define ION_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace PersistingBytes {

/* Warning : All persisting bytes are expected to be initialized to 0 in
 * shared_kernel_flash.ld which assumes PersistingBytesInt is 16 bits long */
typedef uint16_t PersistingBytesInt;

void write(PersistingBytesInt value);
PersistingBytesInt read();

}
}

#endif
