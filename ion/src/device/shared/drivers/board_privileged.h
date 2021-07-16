#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_PRIVILEGED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_PRIVILEGED_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

void init();
void shutdown();
void initFPU();
void initCompensationCell();
void shutdownCompensationCell();
void setDefaultGPIO();

enum class Frequency {
  Low = 0,
  High = 1
};

void setClockLowFrequency();
void setClockStandardFrequency();
void setStandardFrequency(Frequency f);

typedef uint32_t PCBVersion;
/* On N0110 released before the PCB revision, OTP0 is supposed to be blank and
 * unlocked. However, such a device with something written in OTP0 will be
 * unable to configure Vbus properly. In this case, and if OTP0 is still
 * unlocked, we fully write OTP0 and treat it the same as fully blank.
 * This way, pcbVersion will be 0 if OTP0 is either 0x00000000 or 0xFFFFFFFF.*/
constexpr PCBVersion k_alternateBlankVersion = 0xFFFFFFFF;

PCBVersion pcbVersion();
PCBVersion readPCBVersionInMemory();
void writePCBVersion(PCBVersion version);
void lockPCBVersion();
bool pcbVersionIsLocked();
void lockUnlockedPCBVersion();

}
}
}

#endif

