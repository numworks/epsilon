#ifndef ION_DEVICE_SHARED_USERLAND_HEADER_H
#define ION_DEVICE_SHARED_USERLAND_HEADER_H

#include <assert.h>
#include "kernel_header.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace Ion {
namespace Device {

class UserlandHeader {
public:
  constexpr UserlandHeader();
  static bool ValidAddress(uint32_t address) {
    UserlandHeader * info = reinterpret_cast<UserlandHeader *>(address);
    if (info->m_storageAddressRAM == nullptr
      || info->m_storageSizeRAM == 0
      || info->m_externalAppsFlashStart == nullptr
      || info->m_externalAppsFlashEnd == nullptr
      || info->m_externalAppsRAMStart == nullptr
      || info->m_externalAppsRAMEnd == nullptr
      || info->m_header != Magic
      || info->m_footer != Magic) {
      return false;
    }
    return true;
  }
  static void ExtractExpectedEpsilonVersion(uint32_t address, char buffer[KernelHeader::k_epsilonVersionSize]) {
    assert(ValidAddress(address));
    UserlandHeader * info = reinterpret_cast<UserlandHeader *>(address);
    strlcpy(buffer, info->m_expectedEpsilonVersion, KernelHeader::k_epsilonVersionSize);
  }
  void * externalAppsAddressFlash() const {
    return m_externalAppsFlashStart;
  }
  size_t externalAppsSizeFlash() const {
    return static_cast<uint8_t *>(m_externalAppsFlashEnd) - static_cast<uint8_t *>(m_externalAppsFlashStart);
  }
private:
  constexpr static uint32_t Magic = 0xDEC0EDFE;
  uint32_t m_header;
  const char m_expectedEpsilonVersion[8];
  void * m_storageAddressRAM;
  size_t m_storageSizeRAM;
  /* We store the range addresses of external apps memory because storing the
   * size is complicated due to c++11 constexpr. */
  void * m_externalAppsFlashStart;
  void * m_externalAppsFlashEnd;
  void * m_externalAppsRAMStart;
  void * m_externalAppsRAMEnd;
  uint32_t m_footer;
};

}
}

#endif

