#ifndef BOOTLOADER_USERLAND_HEADER_H
#define BOOTLOADER_USERLAND_HEADER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

namespace Bootloader {

class UserlandHeader {
public:
  const char * version() const;
  const bool isValid() const;
  const bool isOmega() const;
  const char * omegaVersion() const;
  const bool isUpsilon() const;
  const char * upsilonVersion() const;

private:
  UserlandHeader();
  constexpr static uint32_t Magic = 0xDEC0EDFE;
  constexpr static uint32_t OmegaMagic = 0xEFBEADDE;
  constexpr static uint32_t UpsilonMagic = 0x55707369;
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

extern const UserlandHeader* s_userlandHeaderA;
extern const UserlandHeader* s_userlandHeaderB;

}

#endif
