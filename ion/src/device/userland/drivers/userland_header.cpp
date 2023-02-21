#include <drivers/board.h>
#include <ion.h>
#include <shared/drivers/board_shared.h>

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

extern "C" {
extern char _external_apps_flash_start;
extern char _external_apps_flash_end;
extern char _external_apps_RAM_start;
extern char _external_apps_RAM_end;
}

namespace Ion {

namespace Device {

constexpr UserlandHeader::UserlandHeader()
    : m_header(Magic),
      m_expectedEpsilonVersion{EPSILON_VERSION},
      m_storageAddressRAM(&Ion::Storage::FileSystem::sharedFileSystem),
      m_storageSizeRAM(Ion::Storage::FileSystem::k_storageSize),
      m_externalAppsFlashStart(&_external_apps_flash_start),
      m_externalAppsFlashEnd(&_external_apps_flash_end),
      m_externalAppsRAMStart(&_external_apps_RAM_start),
      m_externalAppsRAMEnd(&_external_apps_RAM_end),
      m_footer(Magic) {}

constexpr UserlandHeader __attribute__((section(".userland_header"), used))
k_userlandHeader;

}  // namespace Device

const char* epsilonVersion() {
  return Ion::Device::Board::kernelHeader()->epsilonVersion();
}

const char* patchLevel() {
  return Ion::Device::Board::kernelHeader()->patchLevel();
}

}  // namespace Ion
