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

uint32_t userlandEnd() {
  return Config::UserlandStartAddress + Config::UserlandSize;
}

}
}
}
