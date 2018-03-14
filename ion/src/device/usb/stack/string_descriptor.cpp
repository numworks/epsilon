#include "string_descriptor.h"

namespace Ion {
namespace USB {
namespace Device {

uint16_t StringDescriptor::copy(void * target, size_t maxSize) const {
  size_t headerCopySize = copyHeaderOnly(target, maxSize);
  if (!(maxSize > headerCopySize)) {
    // Nothing left to copy
    return headerCopySize;
  }

  /* USB expects UTF-16 encoded strings. We assume m_string will be an ASCII
   * string, so we simply zero-pad m_string before copying it. */
  uint16_t * utf16target = (uint16_t *)((char *)target + headerCopySize);
  const char * stringPointer = m_string;
  size_t bodyCopySize = 0;
  size_t maxBodyCopySize = maxSize - headerCopySize;
  while (*stringPointer != 0 && bodyCopySize <= maxBodyCopySize) {
    *utf16target++ = *stringPointer++;
    bodyCopySize++;
  }

  return headerCopySize + bodyCopySize;
}

}
}
}
