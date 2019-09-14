#ifndef ION_DEVICE_SHARED_SERIAL_NUMBER_H
#define ION_DEVICE_SHARED_SERIAL_NUMBER_H

namespace Ion {
namespace Device {
namespace SerialNumber {

/* The serial number is 96 bits long. That's equal to 16 digits in base 64. We
 * expose a convenient "copySerialNumber" routine which can be called without
 * using a static variable (and therefore without a .bss section). This is used
 * in the RAM'ed DFU bootloader. */
constexpr static int Length = 16;
void copy(char * buffer);

}
}
}

#endif
