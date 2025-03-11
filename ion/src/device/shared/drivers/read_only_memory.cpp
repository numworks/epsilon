#include <stdint.h>

#include "flash_information.h"

namespace Ion {
namespace ReadOnlyMemory {

bool IncludesAddress(const void* address) {
  return Ion::Device::Flash::IncludesAddress(
      reinterpret_cast<uintptr_t>(address));
}

}  // namespace ReadOnlyMemory
}  // namespace Ion
