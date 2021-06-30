#include <drivers/userland_header.h>
#include <drivers/board.h>
#include <ion.h>

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif

extern "C" {
extern char _storage_flash_start;
extern char _storage_flash_end;
}

namespace Ion {

extern char staticStorageArea[];

namespace Device {

constexpr void * storageAddress = &(Ion::staticStorageArea);

constexpr UserlandHeader::UserlandHeader() :
  m_header(Magic),
  m_expectedEpsilonVersion{EPSILON_VERSION},
  m_storageAddressRAM(storageAddress),
  m_storageSizeRAM(Ion::Storage::k_storageSize),
  m_storageAddressFlashStart(&_storage_flash_start),
  m_storageAddressFlashEnd(&_storage_flash_end),
  m_footer(Magic) { }

constexpr UserlandHeader __attribute__((section(".userland_header"),used)) k_userlandHeader;

}

const char * epsilonVersion() {
  return Ion::Device::Board::kernelHeader()->epsilonVersion();
}

const char * patchLevel() {
  return Ion::Device::Board::kernelHeader()->patchLevel();
}

}
