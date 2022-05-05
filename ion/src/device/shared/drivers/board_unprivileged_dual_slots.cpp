#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>

extern "C" {
  extern char _persisting_bytes_buffer_start;
}

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>(Device::Config::KernelOrigin + Device::Config::SignedPayloadLength);
}

UserlandHeader * userlandHeader() {
  return reinterpret_cast<UserlandHeader *>(Device::Config::UserlandOrigin);
}

uint32_t userlandEnd() {
  // On running slot, userland range is USERLAND + SIGNATURE + EXTERN APPS
  // || SP* HEADER | KERNEL | USERLAND | SIGNATURE | EXTERN APPS | PERSISTING BYTES
  return reinterpret_cast<uint32_t>(&_persisting_bytes_buffer_start);
}

}
}
}
