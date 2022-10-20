#include <ion/external_apps.h>
#include <drivers/board.h>
#include <config/board.h>
#include <assert.h>

#ifndef EXTERNAL_APPS_API_LEVEL
#error This file expects EXTERNAL_APPS_API_LEVEL to be defined
#endif

extern "C" {
  extern uint8_t _external_apps_flash_start;
  extern uint8_t _external_apps_flash_end;
}

namespace Ion {
namespace ExternalApps {

/* The ExternalApp start with its info layout as:
 * - 4 bytes: a magic code 0xBABECODE
 * - 4 bytes: the API level of the AppInfo layout
 * - 4 bytes: the address of the app name
 * - 4 bytes: the size of the compressed icon
 * - 4 bytes: the address of the compressed icon data
 * - 4 bytes: the address of the entry point
 * - 4 bytes: the size of the external app including the AppInfo header
 * - 4 bytes: the same magic code 0xBABECODE
 */

constexpr static uint32_t k_magic = 0xDEC0BEBA;
constexpr int k_numberOfAppInfoElements = 8;
constexpr uint32_t k_minAppSize = sizeof(uint32_t) * k_numberOfAppInfoElements;

bool appAtAddress(uint8_t * address) {
  return *reinterpret_cast<uint32_t *>(address) == k_magic;
}

App::App(uint8_t * a) : m_startAddress(a) {
  assert(appAtAddress(m_startAddress));
}

bool addressWithinExternalAppsSection(const uint8_t * address) {
  return address >= &_external_apps_flash_start && address < &_external_apps_flash_end;
}

uint8_t * App::addressAtIndexInAppInfo(int index) const {
  assert(index < k_numberOfAppInfoElements);
  uint8_t * address = m_startAddress + *reinterpret_cast<uint32_t *>(m_startAddress + index*sizeof(uint32_t));
 // Check that address is in authorized memory
  if (!addressWithinExternalAppsSection(address)) {
    return nullptr;
  }
  return address;
}

const uint32_t App::APILevel() const {
  return *reinterpret_cast<const uint32_t *>(m_startAddress + sizeof(uint32_t));
}

const char * App::name() const {
 const char * n = reinterpret_cast<const char *>(addressAtIndexInAppInfo(2));
 if (n == nullptr) {
   return nullptr;
 }
 // Check that all name is within authorized memory
 const char * c = n;
 while (*c != 0) {
   if (!addressWithinExternalAppsSection(reinterpret_cast<const uint8_t *>(c++))) {
     return nullptr;
   }
 }
 return n;
}

uint32_t App::iconSize() const {
  uint32_t size = *reinterpret_cast<uint32_t *>(m_startAddress + 3*sizeof(uint32_t));
  const uint8_t * data = iconData();
  if (data == nullptr || !addressWithinExternalAppsSection(data + size - 1)) {
    return 0;
  }
  return size;
}

const uint8_t * App::iconData() const {
  // TODO: Add check on decompression?
  return reinterpret_cast<const uint8_t *>(addressAtIndexInAppInfo(4));
}

void * App::entryPoint() const {
  if (APILevel() != EXTERNAL_APPS_API_LEVEL) {
    return nullptr;
  }
  /* As stated in ARM Cortex guide generic user guide: Bit[0] of any address
   * you write to the PC with a BX, BLX, LDM, LDR, or POP instruction must be
   * 1 for correct execution, because this bit indicates the required
   * instruction set, and the Cortex-M7 processor only supports Thumb
   * instructions.
   */
  return reinterpret_cast<void *>(reinterpret_cast<uint32_t>(addressAtIndexInAppInfo(5)) | 0x1);
}

uint8_t * nextSectorAlignedAddress(uint8_t * address) {
  // Trick to return address if it was already aligned
  address -= 1;
  // Find previous aligned address
  address = reinterpret_cast<uint8_t *>(reinterpret_cast<uint32_t>(address) & ~(Ion::Device::Board::Config::ExternalAppsSectorUnit - 1));
  address += Ion::Device::Board::Config::ExternalAppsSectorUnit;
  return address;
}

AppIterator & AppIterator::operator++() {
  uint32_t sizeOfCurrentApp = *reinterpret_cast<uint32_t *>(m_currentAddress + 6*sizeof(uint32_t));
  m_currentAddress += sizeOfCurrentApp;
  // Find the next address aligned on external apps sector size
  m_currentAddress = nextSectorAlignedAddress(m_currentAddress);
  if (m_currentAddress < &_external_apps_flash_start || m_currentAddress + k_minAppSize > &_external_apps_flash_end || !appAtAddress(m_currentAddress)) {
    m_currentAddress = nullptr;
  }
  return *this;
}

bool s_externalAppsVisible = false; // After reset, external apps are not visible

AppIterator Apps::begin() const {
  uint8_t * storageStart = &_external_apps_flash_start;
  assert(nextSectorAlignedAddress(storageStart) == storageStart);
  if (!s_externalAppsVisible || !appAtAddress(storageStart)) {
    return end();
  }
  return AppIterator(storageStart);
}

void setVisible() {
  s_externalAppsVisible = true;
  if (numberOfApps() > 0) {
    Device::Board::enableExternalApps(); // Display pop-up
  } else {
    s_externalAppsVisible = false;
  }
}

int numberOfApps() {
  int counter = 0;
  for (App a : Apps()) {
    (void)a;
    counter++;
  }
  return counter;
}

}
}
