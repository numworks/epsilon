#include "function_store.h"

namespace Shared {

uint32_t FunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

}
