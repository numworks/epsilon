#include <ion.h>
#include <assert.h>

#define STR_EXPAND(arg) #arg
#define STR(arg) STR_EXPAND(arg)

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

#ifndef VERSION
#error This file expects VERSION to be defined
#endif

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

class VersionInfo {
public:
  constexpr VersionInfo() :
    m_header(Magic),
    m_version{VERSION},
    m_patchLevel{PATCH_LEVEL},
    m_footer(Magic) { }
  const char * version() const {
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_version;
  }
  const char * patchLevel() const {
    assert(m_header == Magic);
    assert(m_footer == Magic);
    return m_patchLevel;
  }
private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  uint32_t m_footer;
};

constexpr VersionInfo HEADER_SECTION version_infos;

const char * Ion::softwareVersion() {
  return version_infos.version();
}

const char * Ion::patchLevel() {
  return version_infos.patchLevel();
}
