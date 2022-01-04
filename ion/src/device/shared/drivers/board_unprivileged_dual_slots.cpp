#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>
#include <drivers/ram_layout.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>((isRunningSlotA() ? Config::SlotAStartAddress : Config::SlotBStartAddress) +  Board::SignedPayloadSize);
}

UserlandHeader * userlandHeader() {
  uint32_t slotStart = isRunningSlotA() ?  Config::SlotAStartAddress : Config::SlotBStartAddress;
  return reinterpret_cast<UserlandHeader *>(slotStart + Config::UserlandOffset);
}

/* As we want the PCB versions to be in ascending order chronologically, and
 * because the OTP are initialized with 1s, we store the bitwise-not of the
 * version number. This way, devices with blank OTP are considered version 0. */

PCBVersion pcbVersion() {
#if IN_FACTORY
  /* When flashing for the first time, we want all systems that depend on the
   * PCB version to function correctly before flashing the PCB version. This
   * way, flashing the PCB version can be done last. */
  return PCB_LATEST;
#else
  PCBVersion version = readPCBVersionInMemory();
  return (version == k_alternateBlankVersion ? 0 : version);
#endif
}

PCBVersion readPCBVersionInMemory() {
  return ~static_cast<PCBVersion>(InternalFlash::readOTPAtIndex(k_pcbVersionOTPBlock, k_pcbVersionOTPIndex));
}

}
}
}
