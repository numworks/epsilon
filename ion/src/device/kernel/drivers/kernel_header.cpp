#include <drivers/kernel_header.h>

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

namespace Ion {
namespace Device {

constexpr KernelHeader::KernelHeader() :
  m_header(Magic),
  m_epsilonVersion{EPSILON_VERSION},
  m_patchLevel{PATCH_LEVEL},
  m_footer(Magic) { }

constexpr KernelHeader __attribute__((section(".kernel_header"),used)) k_kernelHeader;

}

const char * epsilonVersion() {
  return Device::k_kernelHeader.epsilonVersion();
}

const char * patchLevel() {
  return Device::k_kernelHeader.patchLevel();
}

}
