#ifndef ION_EXTERNAL_APPS_H
#define ION_EXTERNAL_APPS_H

#include <stdint.h>

namespace Ion {
namespace ExternalApps {

class App {
 public:
  App(uint8_t* a);
  const uint32_t APILevel() const;
  const char* name() const;
  uint32_t iconSize() const;
  const uint8_t* iconData() const;
  void* entryPoint() const;
  void eraseMagicCode();

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
  enum class AppInfo : uint8_t {
    MagicStart = 0,
    APILevel = 1,
    NameAddress = 2,
    IconSize = 3,
    IconAddress = 4,
    EntryPointAddress = 5,
    AppSize = 6,
    MagicEnd = 7
  };
  constexpr static uint8_t k_numberOfAppInfoElements =
      static_cast<uint8_t>(App::AppInfo::MagicEnd) + 1;
  constexpr static uint32_t k_minAppSize =
      sizeof(uint32_t) * k_numberOfAppInfoElements;

 private:
  uint32_t appInfo(AppInfo info) const;
  uint8_t* appInfoToAddress(AppInfo info) const;
  uint8_t* m_startAddress;
};

class AppIterator {
 public:
  AppIterator(uint8_t* address) : m_currentAddress(address) {}
  App operator*() { return App(m_currentAddress); }
  AppIterator& operator++();
  bool operator!=(const AppIterator& it) const {
    return m_currentAddress != it.m_currentAddress;
  }

 private:
  uint8_t* m_currentAddress;
};

class Apps {
 public:
  AppIterator begin() const;
  AppIterator end() const { return AppIterator(nullptr); };
};

int numberOfApps();
void deleteApps();
bool allowThirdParty();

}  // namespace ExternalApps
}  // namespace Ion

#endif
