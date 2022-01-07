#ifndef ION_DEVICE_SHARED_USERLAND_HEADER_H
#define ION_DEVICE_SHARED_USERLAND_HEADER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace Ion {
namespace Device {

class UserlandHeader {
public:
  constexpr UserlandHeader();
  bool isValid() const {
    if (m_storageAddressRAM == nullptr
      || m_storageSizeRAM == 0
      || m_externalAppsFlashStart == nullptr
      || m_externalAppsFlashEnd == nullptr
      || m_externalAppsRAMStart == nullptr
      || m_externalAppsRAMEnd == nullptr
      || m_header != Magic
      || m_footer != Magic) {
      return false;
    }
    return true;
  }
  const char * expectedEpsilonVersion() const {
    assert(m_storageAddressRAM != nullptr);
    assert(m_storageSizeRAM != 0);
    assert(m_externalAppsFlashStart != nullptr);
    assert(m_externalAppsFlashEnd != nullptr);
    assert(m_externalAppsRAMStart != nullptr);
    assert(m_externalAppsRAMEnd != nullptr);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_expectedEpsilonVersion;
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

