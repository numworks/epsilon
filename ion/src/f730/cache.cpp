#ifndef ION_DEVICE_CACHE_H
#define ION_DEVICE_CACHE_H

namespace Ion {
namespace Device {
namespace Cache {

void dsb();
void isb();

void enableDCache();
void enableICache();

void invalidateDCache();
void invalidateICache();

}
}
}

#endif
