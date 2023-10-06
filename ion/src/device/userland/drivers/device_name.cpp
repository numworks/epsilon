#include <assert.h>
#include <ion/device_name.h>
#include <string.h>

#include "persisting_bytes.h"

namespace Ion {
namespace DeviceName {

const char* read() {
  const char* name = reinterpret_cast<const char*>(
      PersistingBytes::read(PersistingBytes::Entry::DeviceName));
  assert(strlen(name) <
         PersistingBytes::entrySize(PersistingBytes::Entry::DeviceName));
  return name;
}

}  // namespace DeviceName
}  // namespace Ion
