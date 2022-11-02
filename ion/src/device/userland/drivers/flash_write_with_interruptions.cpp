#include <drivers/svcall.h>
#include <shared/drivers/flash_write_with_interruptions.h>

namespace Ion {
namespace Device {
namespace Flash {

bool SVC_ATTRIBUTES WriteMemoryWithInterruptions(uint8_t * destination, const uint8_t * source, size_t length, bool handleInterruptsAndCache) {
  SVC_RETURNING_R0(SVC_FLASH_WRITE_MEMORY_WITH_INTERRUPTIONS, bool)
}

}
}
}