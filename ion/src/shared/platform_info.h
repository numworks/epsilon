#ifndef ION_DEVICE_SHARED_PLATFORM_INFO_H
#define ION_DEVICE_SHARED_PLATFORM_INFO_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

class PlatformInfo {
public:
  constexpr PlatformInfo();
  bool isValid() const {
    if (m_storageAddress == nullptr
      || m_storageSize == 0
      || m_header != Magic
      || m_footer != Magic) {
      return false;
    }
    for (size_t i = 0; i < sizeof(m_epsilonVersion); i++) {
      if (!(m_epsilonVersion[i] == '.'
           || (m_epsilonVersion[i] >= '0' && m_epsilonVersion[i] <= '9'))) {
        return false;
      }
      assert(sizeof(m_kernelVersion) == sizeof(m_epsilonVersion));
      if (!(m_kernelVersion[i] >= '0' && m_kernelVersion[i] <= '9')) {
        return false;
      }
    }
    if (kernelVersionValue() >= k_kernelMaxVersion) {
      return false;
    }
    return true;
  }
  const char * epsilonVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_epsilonVersion;
  }
  // TODO: add tests
  int epsilonVersionComparedTo(const char * version) {
    const char * previousDotPosition = m_epsilonVersion;
    const char * comparedPreviousDotPosition = version;
    while (true) {
      const char * dotPosition = strchr(previousDotPosition + 1, '.');
      const char * comparedDotPosition = strchr(comparedPreviousDotPosition + 1, '.');
      if (dotPosition == nullptr) {
        assert(comparedDotPosition == nullptr);
        dotPosition = strchr(previousDotPosition, 0);
        comparedDotPosition = strchr(comparedPreviousDotPosition, 0);
      }
      size_t deltaDot = dotPosition - previousDotPosition;
      size_t compareDeltaDot = comparedDotPosition - comparedPreviousDotPosition;

      if (deltaDot != compareDeltaDot) {
        return deltaDot > compareDeltaDot;
      } else {
        for (size_t i = 0; i < deltaDot; i++) {
          if (*previousDotPosition != *comparedPreviousDotPosition) {
            return *previousDotPosition - *comparedPreviousDotPosition;
          }
          previousDotPosition++;
          comparedPreviousDotPosition++;
        }
      }
      if (*dotPosition == 0) {
        return 0;
      }
      previousDotPosition = dotPosition;
      comparedPreviousDotPosition = comparedDotPosition;
    }
  }
  const char * kernelVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_kernelVersion;
  }
  uint32_t kernelVersionValue() const {
    const char * current = m_kernelVersion;
    uint32_t result = 0;
    while (*current != 0) {
      result = 10 * result + (*current++) - '0';
    }
    return result;
  }
  const char * patchLevel() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_patchLevel;
  }
private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  constexpr static uint32_t k_kernelMaxVersion = 15*32*8; // NumberOfAvailableOTPBlocks * BytesPerBlock * BitsPerBytes
  uint32_t m_header;
  const char m_epsilonVersion[8];
  const char m_kernelVersion[8];
  const char m_patchLevel[8];
  void * m_storageAddress;
  size_t m_storageSize;
  uint32_t m_footer;
};

#endif
