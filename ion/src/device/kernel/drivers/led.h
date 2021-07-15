#ifndef ION_SRC_DEVICE_KERNEL_DRIVERS_LED_H
#define ION_SRC_DEVICE_KERNEL_DRIVERS_LED_H

#include <kandinsky/color.h>
#include <shared/drivers/led.h>

namespace Ion {
namespace Device {
namespace LED {

void setColorSecure(KDColor c);

}
}
}

#endif
