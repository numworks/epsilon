#include <ion.h>
#include <assert.h>

#ifndef PATCH_LEVEL
#error This file expects PATCH_LEVEL to be defined
#endif

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

#ifndef OMEGA_VERSION
#error This file expects OMEGA_VERSION to be defined
#endif

#ifndef HEADER_SECTION
#define HEADER_SECTION
#endif

namespace Ion {
extern char staticStorageArea[];
}
constexpr void * storageAddress = &(Ion::staticStorageArea);

class PlatformInfo {
public:
  constexpr PlatformInfo() :
    m_header(Magic),
    m_version{EPSILON_VERSION},
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
  constexpr static uint32_t OmegaMagic = 0xEFBEADDE;
  uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  uint32_t m_footer;
};

const PlatformInfo HEADER_SECTION platform_infos;

const char * Ion::softwareVersion() {
  return platform_infos.version();
}

static const char s_omegaVersion[16] = {OMEGA_VERSION};
#ifdef OMEGA_USERNAME
    static const char s_username[16] = {OMEGA_USERNAME};
#else
    static const char s_username[16] = {"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"};
#endif

const char * Ion::omegaVersion() {
  return s_omegaVersion;
}

const volatile char * Ion::username() {
  return s_username;
}

const char * Ion::patchLevel() {
  return platform_infos.patchLevel();
}
