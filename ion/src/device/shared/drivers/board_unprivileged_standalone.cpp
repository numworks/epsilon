#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>
#include <shared/boot/isr.h>

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
