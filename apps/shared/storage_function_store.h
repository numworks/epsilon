#ifndef SHARED_STORAGE_FUNCTION_STORE_H
#define SHARED_STORAGE_FUNCTION_STORE_H

#include "function.h"
#include "storage_expression_model_store.h"
#include <stdint.h>

namespace Shared {

/* FunctionStore storse functions and gives them a color. */

class FunctionStore : public StorageExpressionModelStore {
public:
  FunctionStore() : StorageExpressionModelStore() {}
  uint32_t storeChecksum();
  // An active function must be defined to be counted
  int numberOfActiveFunctions() const { return numberOfModelsSatisfyingTest([](ExpressionModelHandle * m) { return m->isDefined() && static_cast<Function *>(m)->isActive(); }); }
  Ion::Storage::Record activeRecordAtIndex(int i) const { return recordStatifyingTestAtIndex(i, [](ExpressionModelHandle * m) { return m->isDefined() && static_cast<Function *>(m)->isActive(); }); }

  ExpiringPointer<Function> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<Function>(static_cast<Function *>(privateModelForRecord(record))); }

};

}

#endif
