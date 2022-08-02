#include <config/board.h>
#include <shared/boot/isr.h>
#include <shared/drivers/board_shared.h>
#include <shared/drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>(Config::KernelVirtualOrigin + INITIALISATION_VECTOR_SIZE * sizeof(void *));
}

UserlandHeader * userlandHeader() {
  return reinterpret_cast<UserlandHeader *>(Config::UserlandVirtualOrigin);
}

uint32_t userlandEnd() {
  return Config::UserlandVirtualOrigin + Config::UserlandLength;
}

}
}
}
