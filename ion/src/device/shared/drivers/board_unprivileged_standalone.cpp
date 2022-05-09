#include <config/board.h>
#include <drivers/board.h>
#include <drivers/kernel_header.h>

namespace Ion {
namespace Device {
namespace Board {

KernelHeader * kernelHeader() {
  return reinterpret_cast<KernelHeader *>(Config::KernelVirtualOrigin + Config::SignedPayloadLength);
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
