#include "storage_function_store.h"
#include "storage_cartesian_function.h"
#include <assert.h>

namespace Shared {

uint32_t StorageFunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

}
