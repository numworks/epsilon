#ifndef SHARED_STORAGE_FUNCTION_STORE_H
#define SHARED_STORAGE_FUNCTION_STORE_H

#include "storage_function.h"
#include "storage_expression_model_store.h"
#include <stdint.h>

namespace Shared {

/* FunctionStore storse functions and gives them a color. */

class StorageFunctionStore : public StorageExpressionModelStore {
public:
  StorageFunctionStore() : StorageExpressionModelStore() {}
  uint32_t storeChecksum();
  // An active function must be defined to be counted
  int numberOfActiveFunctions() const { return numberOfModelsSatisfyingTest([](ExpressionModelHandle * m) { return m->isDefined() && static_cast<StorageFunction *>(m)->isActive(); }); }
  Ion::Storage::Record activeRecordAtIndex(int i) const { return recordStatifyingTestAtIndex(i, [](ExpressionModelHandle * m) { return m->isDefined() && static_cast<StorageFunction *>(m)->isActive(); }); }

  ExpiringPointer<StorageFunction> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<StorageFunction>(static_cast<StorageFunction *>(privateModelForRecord(record))); }

  virtual char symbol() const = 0;
};

}

#endif
