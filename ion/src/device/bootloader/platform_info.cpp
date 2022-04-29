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

namespace Ion {
extern char staticStorageArea[];
}
constexpr void * storageAddress = &(Ion::staticStorageArea);

class KernelHeader {
public:
  constexpr KernelHeader() :
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
  uint32_t m_header;
  const char m_version[8];
  const char m_patchLevel[8];
  uint32_t m_footer;
};

const KernelHeader __attribute__((section(".kernel_header"), used)) k_kernelHeader;

class UserlandHeader {
public:
  constexpr UserlandHeader():
    m_header(Magic),
    m_expectedEpsilonVersion{EPSILON_VERSION},
    m_storageAddressRAM(storageAddress),
    m_storageSizeRAM(Ion::Storage::k_storageSize),
    m_externalAppsFlashStart(0xFFFFFFFF),
    m_externalAppsFlashEnd(0xFFFFFFFF),
    m_externalAppsRAMStart(0xFFFFFFFF),
    m_externalAppsRAMEnd(0xFFFFFFFF),
    m_footer(Magic),
    m_omegaMagicHeader(OmegaMagic),
    m_omegaVersion{OMEGA_VERSION},
#ifdef OMEGA_USERNAME
    m_username{OMEGA_USERNAME},
#else
    m_username{"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"},
#endif
    m_omegaMagicFooter(OmegaMagic),
    m_upsilonMagicHeader(UpsilonMagic),
    m_UpsilonVersion{UPSILON_VERSION},
    m_osType(OSType),
    m_upsilonMagicFooter(UpsilonMagic) { }

  const char * omegaVersion() const {
    assert(m_storageAddressRAM != nullptr);
    assert(m_storageSizeRAM != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_omegaMagicHeader == OmegaMagic);
    assert(m_omegaMagicFooter == OmegaMagic);
    return m_omegaVersion;
  }
  const char * upsilonVersion() const {
    assert(m_storageAddress != nullptr);
    assert(m_storageSize != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_omegaMagicHeader == OmegaMagic);
    assert(m_omegaMagicFooter == OmegaMagic);
    return m_UpsilonVersion;
  }
  const volatile char * username() const volatile {
    assert(m_storageAddressRAM != nullptr);
    assert(m_storageSizeRAM != 0);
    assert(m_header == Magic);
    assert(m_footer == Magic);
    assert(m_omegaMagicHeader == OmegaMagic);
    assert(m_omegaMagicFooter == OmegaMagic);
    return m_username;
  }
  const void * storage_address() const {
    return storageAddress;
  }
private:
  constexpr static uint32_t Magic = 0xDEC0EDFE;
  constexpr static uint32_t OmegaMagic = 0xEFBEADDE;
  constexpr static uint32_t UpsilonMagic = 0x55707369;
  constexpr static uint32_t OSType = 0x79827178;
  uint32_t m_header;
  const char m_expectedEpsilonVersion[8];
  void * m_storageAddressRAM;
  size_t m_storageSizeRAM;
  /* We store the range addresses of external apps memory because storing the
   * size is complicated due to c++11 constexpr. */
  uint32_t m_externalAppsFlashStart;
  uint32_t m_externalAppsFlashEnd;
  uint32_t m_externalAppsRAMStart;
  uint32_t m_externalAppsRAMEnd;
  uint32_t m_footer;
  uint32_t m_omegaMagicHeader;
  const char m_omegaVersion[16];
  const volatile char m_username[16];
  uint32_t m_omegaMagicFooter;
  uint32_t m_upsilonMagicHeader;
  const char m_UpsilonVersion[16];
  uint32_t m_osType;
  uint32_t m_upsilonMagicFooter;
};

const UserlandHeader __attribute__((section(".userland_header"), used)) k_userlandHeader;

class SlotInfo {

public:
  SlotInfo() :
    m_header(Magic),
    m_footer(Magic) {}
  void update() {
    m_header = Magic;
    m_kernelHeaderAddress = &k_kernelHeader;
    m_userlandHeaderAddress = &k_userlandHeader;
    m_footer = Magic;
  }

private:
  constexpr static uint32_t Magic = 0xEFEEDBBA;
  uint32_t m_header;
  const KernelHeader * m_kernelHeaderAddress;
  const UserlandHeader * m_userlandHeaderAddress;
  uint32_t m_footer;

};

const char * Ion::omegaVersion() {
  return k_userlandHeader.omegaVersion();
}

const char * Ion::upsilonVersion() {
  return k_userlandHeader.upsilonVersion();
}

const volatile char * Ion::username() {
  return k_userlandHeader.username();
}

const char * Ion::softwareVersion() {
  return k_kernelHeader.version();
}

const char * Ion::patchLevel() {
  return k_kernelHeader.patchLevel();
}

const void * Ion::storageAddress() {
  return k_userlandHeader.storage_address();
}

SlotInfo * slotInfo() {
  static SlotInfo __attribute__((used)) __attribute__((section(".slot_info"))) slotInformation;
  return &slotInformation;
}

void Ion::updateSlotInfo() {
  slotInfo()->update();
}
