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
      || m_storageAddressFlashStart == nullptr
      || m_storageAddressFlashEnd == nullptr
      || m_header != Magic
      || m_footer != Magic) {
      return false;
    }
    return true;
  }
  const char * expectedEpsilonVersion() const {
    assert(m_storageAddressRAM != nullptr);
    assert(m_storageSizeRAM != 0);
    assert(m_storageAddressFlashStart != nullptr);
    assert(m_storageAddressFlashEnd != nullptr);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_expectedEpsilonVersion;
  }
  void * storageAddressFlash() const {
    return m_storageAddressFlashStart;
  }
  size_t storageSizeFlash() const {
    return static_cast<uint8_t *>(m_storageAddressFlashEnd) - static_cast<uint8_t *>(m_storageAddressFlashStart);
  }
private:
  constexpr static uint32_t Magic = 0xDEC0EDFE;
  uint32_t m_header;
  const char m_expectedEpsilonVersion[8];
  void * m_storageAddressRAM;
  size_t m_storageSizeRAM;
  /* We store the range addresses of flash storage because storing the size is
   * complicated due to c++11 constexpr. */
  void * m_storageAddressFlashStart;
  void * m_storageAddressFlashEnd;
  uint32_t m_footer;
};

}
}

#endif

