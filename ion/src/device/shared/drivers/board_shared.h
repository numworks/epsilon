#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_SHARED_H

#include <drivers/kernel_header.h>
#include <drivers/userland_header.h>
#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

void init();
void initFPU();
void initMPU();
void initSystemClocks();
void initPeripheralsClocks();
void initCompensationCell();
void initInterruptions();

void shutdown();
void shutdownCompensationCell();
void setDefaultGPIO();

void setActivePLLConfig();
void setDeactivatedPLLConfig();

enum class Frequency {
  Low = 0,
  High = 1
};
void setFrequency(Frequency f);
void forceFrequency(Frequency f);
void resetThreadModeFrequency();
void setFrequencyWithoutSystick(Frequency f);
void setLowVoltageScaling();
void setHighVoltageScaling();

KernelHeader * kernelHeader();
UserlandHeader * userlandHeader();
uint32_t userlandStart();
bool isRunningSlotA();

typedef uint32_t PCBVersion;
/* On N0110 released before the PCB revision, OTP0 is supposed to be blank and
 * unlocked. However, such a device with something written in OTP0 will be
 * unable to configure Vbus properly. In this case, and if OTP0 is still
 * unlocked, we fully write OTP0 and treat it the same as fully blank.
 * This way, pcbVersion will be 0 if OTP0 is either 0x00000000 or 0xFFFFFFFF.*/
constexpr PCBVersion k_alternateBlankVersion = 0xFFFFFFFF;
PCBVersion pcbVersion();
PCBVersion readPCBVersionInMemory();
bool pcbVersionIsLocked();
void lockPCBVersion();
void writePCBVersion(PCBVersion version);

uint32_t writableSRAMEndAddress();
uint32_t writableSRAMStartAddress();

}
}
}

#endif
