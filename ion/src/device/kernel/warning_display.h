#ifndef ION_DEVICE_KERNEL_WARNING_DISPLAY_H
#define ION_DEVICE_KERNEL_WARNING_DISPLAY_H

#include <shared/warning_display.h>

namespace Ion {
namespace Device {
namespace WarningDisplay {

void unauthenticatedUserland();
void obsoleteSoftware();
void kernelUpgradeRequired();

}
}
}

#endif
