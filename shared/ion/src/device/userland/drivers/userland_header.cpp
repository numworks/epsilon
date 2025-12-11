#include <ion.h>
#include <shared/drivers/board_shared.h>

#include "board.h"
#include "persisting_bytes.h"

#ifndef SOFTWARE_VERSION
#error This file expects SOFTWARE_VERSION to be defined
#endif

extern "C" {
extern char _external_apps_flash_start;
extern char _external_apps_flash_end;
extern char _external_apps_RAM_start;
extern char _external_apps_RAM_end;
extern char _device_name_sector_start;
extern char _device_name_sector_end;
}

namespace Ion {

namespace Device {

constexpr UserlandHeader::UserlandHeader()
    : m_header(Magic),
      m_expectedSoftwareVersion{SOFTWARE_VERSION},
      m_storageAddressRAM(&Ion::Storage::FileSystem::sharedFileSystem),
      m_storageSizeRAM(Ion::Storage::FileSystem::k_totalSize),
      m_externalAppsFlashStart(&_external_apps_flash_start),
      m_externalAppsFlashEnd(&_external_apps_flash_end),
      m_externalAppsRAMStart(&_external_apps_RAM_start),
      m_externalAppsRAMEnd(&_external_apps_RAM_end),
      m_deviceNameFlashStart(&_device_name_sector_start),
      m_deviceNameFlashEnd(&_device_name_sector_end),
      m_footer(Magic) {}

constexpr UserlandHeader __attribute__((section(".userland_header"), used))
k_userlandHeader;

}  // namespace Device

const char* softwareVersion() {
  return Ion::Device::Board::kernelHeader()->softwareVersion();
}

const char* commitHash() {
  return Ion::Device::Board::kernelHeader()->commitHash();
}

}  // namespace Ion
