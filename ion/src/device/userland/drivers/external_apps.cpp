#include <assert.h>
#include <config/board.h>
#include <drivers/board.h>
#include <ion/external_apps.h>
#include <shared/drivers/flash_write_with_interruptions.h>

#ifndef EXTERNAL_APPS_API_LEVEL
#error This file expects EXTERNAL_APPS_API_LEVEL to be defined
#endif

extern "C" {
extern uint8_t _external_apps_flash_start;
extern uint8_t _external_apps_flash_end;
}

namespace Ion {
namespace ExternalApps {

constexpr static uint32_t k_magic = 0xDEC0BEBA;

bool appAtAddress(uint8_t* address) {
  return *reinterpret_cast<uint32_t*>(address) == k_magic;
}

App::App(uint8_t* a) : m_startAddress(a) {
  assert(appAtAddress(m_startAddress));
}

bool addressWithinExternalAppsSection(const uint8_t* address) {
  return address >= &_external_apps_flash_start &&
         address < &_external_apps_flash_end;
}

uint32_t App::appInfo(AppInfo info) const {
  return reinterpret_cast<uint32_t*>(
      m_startAddress)[static_cast<uint8_t>(info)];
}

uint8_t* App::appInfoToAddress(AppInfo info) const {
  assert(info == AppInfo::NameAddress || info == AppInfo::IconAddress ||
         info == AppInfo::EntryPointAddress);
  uint8_t* address = m_startAddress + appInfo(info);
  // Check that address is in authorized memory
  return addressWithinExternalAppsSection(address) ? address : nullptr;
}

const uint32_t App::APILevel() const { return appInfo(AppInfo::APILevel); }

const char* App::name() const {
  const char* n =
      reinterpret_cast<const char*>(appInfoToAddress(AppInfo::NameAddress));
  if (n == nullptr) {
    return nullptr;
  }
  // Check that all name is within authorized memory
  const char* c = n;
  while (*c != 0) {
    if (!addressWithinExternalAppsSection(
            reinterpret_cast<const uint8_t*>(c++))) {
      return nullptr;
    }
  }
  return n;
}

uint32_t App::iconSize() const {
  uint32_t size = appInfo(AppInfo::IconSize);
  const uint8_t* data = iconData();
  if (data == nullptr || !addressWithinExternalAppsSection(data + size - 1)) {
    return 0;
  }
  return size;
}

const uint8_t* App::iconData() const {
  // TODO: Add check on decompression?
  return reinterpret_cast<const uint8_t*>(
      appInfoToAddress(AppInfo::IconAddress));
}

void* App::entryPoint() const {
  if (APILevel() != EXTERNAL_APPS_API_LEVEL) {
    return nullptr;
  }
  /* As stated in ARM Cortex guide generic user guide: Bit[0] of any address
   * you write to the PC with a BX, BLX, LDM, LDR, or POP instruction must be
   * 1 for correct execution, because this bit indicates the required
   * instruction set, and the Cortex-M7 processor only supports Thumb
   * instructions.
   */
  return reinterpret_cast<void*>(
      reinterpret_cast<uint32_t>(appInfoToAddress(AppInfo::EntryPointAddress)) |
      0x1);
}

void App::eraseMagicCode() {
  assert(appAtAddress(m_startAddress));
  uint8_t value = 0x00;
  Ion::Device::Flash::WriteMemoryWithInterruptions(
      m_startAddress, reinterpret_cast<uint8_t*>(&value), sizeof(value), true);
}

uint8_t* nextSectorAlignedAddress(uint8_t* address) {
  // Trick to return address if it was already aligned
  address -= 1;
  // Find previous aligned address
  address = reinterpret_cast<uint8_t*>(
      reinterpret_cast<uint32_t>(address) &
      ~(Ion::Device::Board::Config::ExternalAppsSectorUnit - 1));
  address += Ion::Device::Board::Config::ExternalAppsSectorUnit;
  return address;
}

AppIterator& AppIterator::operator++() {
  uint32_t sizeOfCurrentApp =
      *reinterpret_cast<uint32_t*>(m_currentAddress + 6 * sizeof(uint32_t));
  m_currentAddress += sizeOfCurrentApp;
  // Find the next address aligned on external apps sector size
  m_currentAddress = nextSectorAlignedAddress(m_currentAddress);
  if (m_currentAddress < &_external_apps_flash_start ||
      m_currentAddress + App::k_minAppSize > &_external_apps_flash_end ||
      !appAtAddress(m_currentAddress)) {
    m_currentAddress = nullptr;
  }
  return *this;
}

AppIterator Apps::begin() const {
  uint8_t* storageStart = &_external_apps_flash_start;
  assert(nextSectorAlignedAddress(storageStart) == storageStart);
  if (!appAtAddress(storageStart)) {
    return end();
  }
  return AppIterator(storageStart);
}

int numberOfApps() {
  int counter = 0;
  for (App a : Apps()) {
    (void)a;
    counter++;
  }
  return counter;
}

void deleteApps() {
  for (App a : Apps()) {
    a.eraseMagicCode();
  }
}

bool allowThirdParty() { return true; }

}  // namespace ExternalApps
}  // namespace Ion
