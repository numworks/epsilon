#ifndef ION_DEVICE_SHARED_KERNEL_HEADER_H
#define ION_DEVICE_SHARED_KERNEL_HEADER_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace Ion {
namespace Device {

class KernelHeader {
 public:
  constexpr KernelHeader();
  const char* epsilonVersion() const {
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_epsilonVersion;
  }
  // TODO: add tests
  int epsilonVersionComparedTo(const char* version) {
    const char* previousDotPosition = m_epsilonVersion;
    const char* comparedPreviousDotPosition = version;
    while (true) {
      const char* dotPosition = strchr(previousDotPosition + 1, '.');
      const char* comparedDotPosition =
          strchr(comparedPreviousDotPosition + 1, '.');
      if (dotPosition == nullptr) {
        assert(comparedDotPosition == nullptr);
        dotPosition = strchr(previousDotPosition, 0);
        comparedDotPosition = strchr(comparedPreviousDotPosition, 0);
      }
      size_t deltaDot = dotPosition - previousDotPosition;
      size_t compareDeltaDot =
          comparedDotPosition - comparedPreviousDotPosition;

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
  const char* patchLevel() const {
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_patchLevel;
  }
  constexpr static size_t k_epsilonVersionSize = 8;

 private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  uint32_t m_header;
  const char m_epsilonVersion[k_epsilonVersionSize];
  const char m_patchLevel[8];
  uint32_t m_footer;
};

}  // namespace Device
}  // namespace Ion

#endif
