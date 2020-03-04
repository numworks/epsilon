#include "function_store.h"

namespace Shared {

uint32_t FunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

uint32_t FunctionStore::storeChecksumAtIndex(size_t i) {
  if (numberOfActiveFunctions() <= i) {
    return 0;
  }
  return activeRecordAtIndex(i).checksum();
}

}
