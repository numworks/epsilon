#ifndef ION_DEVICE_SHARED_PLATFORM_INFO_H
#define ION_DEVICE_SHARED_PLATFORM_INFO_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

class PlatformInfo {
public:
  constexpr PlatformInfo();
  const char * epsilonVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_epsilonVersion;
  }
  int epsilonVersionValue() const { return valueOfStringVersion(epsilonVersion()); }
  const char * kernelVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_kernelVersion;
  }
  int kernelVersionValue() const { return valueOfStringVersion(kernelVersion()); }
  const char * patchLevel() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_patchLevel;
  }
private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  inline int valueOfStringVersion(const char * stringVersion) const { return value(stringVersion[0]) + value(stringVersion[2]) + value(stringVersion[4]); }
  inline int value(char c) const { return c - '0'; }
  uint32_t m_header;
  const char m_epsilonVersion[8];
  const char m_kernelVersion[8];
  const char m_patchLevel[8];
  void * m_storageAddress;
  size_t m_storageSize;
  uint32_t m_footer;
};

#endif
