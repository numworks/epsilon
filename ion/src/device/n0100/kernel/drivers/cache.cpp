#include <shared/drivers/cache.h>

// No L1-cache on N0100

namespace Ion {
namespace Device {
namespace Cache {

void enable() {}
void disable() {}

void invalidateDCache() {}
void cleanDCache() {}
void enableDCache() {}
void disableDCache() {}

void invalidateICache() {}
void enableICache() {}
void disableICache() {}

}
}
}
