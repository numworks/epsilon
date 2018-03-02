#include <ion.h>
#include <assert.h>

#define STR_EXPAND(arg) #arg
#define STR(arg) STR_EXPAND(arg)

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

#ifndef FORCE_LINK
#define FORCE_LINK
#endif

extern Ion::Storage storage;

class PlatformInfo {
public:
  constexpr PlatformInfo() :
    m_header(Magic),
    m_version{EPSILON_VERSION},
    m_patchLevel{PATCH_LEVEL},
    m_storageAddress(&storage),
    m_footer(Magic) { }
  const char * version() const {
    assert(m_storageAddress != nullptr);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_version;
  }
  const char * patchLevel() const {
    assert(m_storageAddress != nullptr);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_patchLevel;
  }
private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  void * m_storageAddress;
  uint32_t m_footer;
};

constexpr PlatformInfo HEADER_SECTION platform_infos;

const char * Ion::softwareVersion() {
  return platform_infos.version();
}

const char * Ion::patchLevel() {
  return platform_infos.patchLevel();
}
