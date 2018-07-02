#ifndef ION_DEVICE_H
#define ION_DEVICE_H

namespace Ion {
namespace Device {

void init();
void shutdown();

void initFPU();
void coreReset();
void jumpReset();

void initPeripherals();
void shutdownPeripherals();
void initClocks();
void shutdownClocks();

/* The serial number is 96 bits long. That's equal to 16 digits in base 64. We
 * expose a convenient "copySerialNumber" routine which can be called without
 * using a static variable (and therefore without a .bss section). This is used
 * in the RAM'ed DFU bootloader. */
constexpr static int SerialNumberLength = 16;
void copySerialNumber(char * buffer);

}
}

#endif
