#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>(Config::KernelHeaderAddress);
}

UserlandHeader * userlandHeader() {
  return reinterpret_cast<UserlandHeader *>(Config::UserlandStartAddress);
}

/* The following methods regarding PCB version are dummy implementations.
 * Handling the PCB version is only necessary on the N0110 and N0120. */

PCBVersion pcbVersion() {
  return PCB_LATEST;
}

PCBVersion readPCBVersionInMemory() {
  return PCB_LATEST;
}

}
}
}
