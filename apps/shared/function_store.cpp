#include "function_store.h"

namespace Shared {

uint32_t FunctionStore::storeChecksum() {
  return Ion::Storage::sharedStorage()->checksum();
}

uint32_t FunctionStore::storeChecksumAtIndex(int i) {
  if (numberOfActiveFunctions() <= i) {
    return 0;
  }
  return activeRecordAtIndex(i).checksum();
}

}
