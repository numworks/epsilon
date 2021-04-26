#include <ion/external_apps.h>
#include <assert.h>

extern "C" {
  uint8_t _external_apps_start;
  uint8_t _external_apps_end;
}

namespace Ion {
namespace ExternalApps {

/* The ExternalApp start with its info layout as:
 * - 4 bytes: a magic code 0XBABECODE
 * - 4 bytes: the API level of the AppInfo layout
 * - 4 bytes: the address of the app name
 * - 4 bytes: the size of the compressed icon
 * - 4 bytes: the address of the compressed icon data
 * - 4 bytes: the address of the entry point
 * - 4 bytes: the size of the external app including the AppInfo header
 * - 4 bytes: the same magic code 0xBABECODE
 */

constexpr int k_numberOfAppInfoElements = 8;
constexpr uint32_t k_minAppSize = sizeof(uint32_t) * k_numberOfAppInfoElements;

App::App(uint8_t * a) : m_startAddress(a) {
  assert(*reinterpret_cast<uint32_t *>(m_startAddress) == k_magic);
}

bool addressWithinExternalAppsSection(const uint8_t * address) {
  return address >= &_external_apps_start && address < &_external_apps_end;
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
  return reinterpret_cast<void *>(addressAtIndexInAppInfo(5));
}

bool App::appAtAddress(uint8_t * address) {
  return *reinterpret_cast<uint32_t *>(address) == k_magic;
}

AppIterator & AppIterator::operator++() {
  uint32_t sizeOfCurrentApp = *reinterpret_cast<uint32_t *>(m_currentAddress + 6*sizeof(uint32_t));
  m_currentAddress += sizeOfCurrentApp;
  if (m_currentAddress < &_external_apps_start || m_currentAddress > &_external_apps_end  - k_minAppSize || !App::appAtAddress(m_currentAddress)) {
    m_currentAddress = nullptr;
  }
  return *this;
}

AppIterator Apps::begin() const {
  if (!App::appAtAddress(&_external_apps_start)) {
    return end();
  }
  return AppIterator(&_external_apps_start);
}

int numberOfApps() {
  int counter = 0;
  for (App a : Apps()) {
    counter++;
  }
  return counter;
}

}
}
