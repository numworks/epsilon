#include <drivers/board_unprivileged.h>
#include <drivers/config/board.h>
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

}
}
}
