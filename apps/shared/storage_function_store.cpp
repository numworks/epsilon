#include "storage_function_store.h"
#include "cartesian_function.h"
#include <assert.h>

namespace Shared {

uint32_t FunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

}
