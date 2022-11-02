#include <drivers/svcall.h>
#include <shared/drivers/flash_write.h>

namespace Ion {
namespace Device {
namespace Flash {

bool SVC_ATTRIBUTES WriteMemory(uint8_t * destination, const uint8_t * source, size_t length) {
  SVC_RETURNING_R0(SVC_FLASH_WRITE_MEMORY, bool)
}

}
}
}