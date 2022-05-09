#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>(Device::Config::KernelVirtualOrigin + Device::Config::SignedPayloadLength);
}

UserlandHeader * userlandHeader() {
  return reinterpret_cast<UserlandHeader *>(Device::Config::UserlandVirtualOrigin);
}

uint32_t userlandEnd() {
  return Device::Config::UserlandVirtualOrigin + Device::Config::UserlandLength;
}

}
}
}
