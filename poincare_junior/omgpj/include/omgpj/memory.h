#ifndef UTILS_MEMORY_H
#define UTILS_MEMORY_H

#include <stddef.h>
#include <stdint.h>

namespace Memory {

size_t AlignedSize(size_t realSize, size_t alignment);
bool Rotate(uint8_t * dst, uint8_t * src, size_t len);

}

#endif
