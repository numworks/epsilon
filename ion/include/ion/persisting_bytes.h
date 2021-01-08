#ifndef ION_PERSISTING_BYTES_H
#define ION_PERSISTING_BYTES_H

#include <stdint.h>

namespace Ion {
namespace PersistingBytes {

void write(uint8_t byte);
uint8_t read();

}
}

#endif
