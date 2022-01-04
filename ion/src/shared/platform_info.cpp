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

#ifndef UPSILON_VERSION
#error This file expects UPSILON_VERSION to be defined
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
    m_storageAddress(storageAddress),
    m_storageSize(Ion::Storage::k_storageSize),
    m_footer(Magic),
    m_ohm_header(OmegaMagic),
    m_OmegaVersion{OMEGA_VERSION},
#ifdef OMEGA_USERNAME
    m_username{OMEGA_USERNAME},
#else
    m_username{"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"},
#endif
    m_ohm_footer(OmegaMagic),
    m_ups_header(UpsilonMagic),
    m_UpsilonVersion{UPSILON_VERSION},
    m_osType(OSType),
    m_ups_footer(UpsilonMagic) { }
  const char * version() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_ohm_header == OmegaMagic);
    assert(m_ohm_footer == OmegaMagic);
    return m_version;
  }
  const char * UpsilonVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_ohm_header == OmegaMagic);
    assert(m_ohm_footer == OmegaMagic);
    return m_UpsilonVersion;
  }  
  const char * OmegaVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_ohm_header == OmegaMagic);
    assert(m_ohm_footer == OmegaMagic);
    return m_OmegaVersion;
  }
  const volatile char * username() const volatile {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_ohm_header == OmegaMagic);
    assert(m_ohm_footer == OmegaMagic);
    return m_username;
  }
  const char * patchLevel() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_ohm_header == OmegaMagic);
    assert(m_ohm_footer == OmegaMagic);
    return m_patchLevel;
  }
private:
  constexpr static uint32_t Magic = 0xDEC00DF0;
  constexpr static uint32_t OmegaMagic = 0xEFBEADDE;
  constexpr static uint32_t UpsilonMagic = 0x55707369;
  constexpr static uint32_t OSType = 0x79827178;
  uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  void * m_storageAddress;
  size_t m_storageSize;
  uint32_t m_footer;
  uint32_t m_ohm_header;
  const char m_OmegaVersion[16];
  const volatile char m_username[16];
  uint32_t m_ohm_footer;
  uint32_t m_ups_header;
  const char m_UpsilonVersion[16];
  uint32_t m_osType;
  uint32_t m_ups_footer;

};

const PlatformInfo HEADER_SECTION platform_infos;

const char * Ion::softwareVersion() {
  return platform_infos.version();
}

const char * Ion::UpsilonVersion() {
  return platform_infos.UpsilonVersion();
}

const char * Ion::OmegaVersion() {
  return platform_infos.OmegaVersion();
}

const volatile char * Ion::username() {
  return platform_infos.username();
}

const char * Ion::patchLevel() {
  return platform_infos.patchLevel();
}
