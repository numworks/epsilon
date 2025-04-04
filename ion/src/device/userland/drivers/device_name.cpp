#include <assert.h>
#include <ion/device_name.h>
#include <string.h>
#if CUSTOM_FIRMWARE
#include <ion/authentication.h>
#endif

#include "persisting_bytes.h"

namespace Ion {
namespace DeviceName {

const char* read() {
#if CUSTOM_FIRMWARE
  assert(Authentication::clearanceLevel() ==
         Authentication::ClearanceLevel::ThirdParty);
  return "";
#else
  const char* name = reinterpret_cast<const char*>(
      PersistingBytes::read(PersistingBytes::Entry::DeviceName));
  assert(strlen(name) <
         PersistingBytes::entrySize(PersistingBytes::Entry::DeviceName));
  return name;
#endif
}

}  // namespace DeviceName
}  // namespace Ion
