#ifndef ION_TIMING_H
#define ION_TIMING_H

#include <stdint.h>

namespace Ion {
namespace Timing {

void usleep(uint32_t us);
void msleep(uint32_t ms);

/* millis is the number of milliseconds ellapsed since a random epoch.
 * On the device, epoch is the boot time. */
uint64_t millis();

}  // namespace Timing
}  // namespace Ion

#endif
