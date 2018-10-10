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
  int numberOfActiveFunctions() const;
  virtual StorageFunction * modelAtIndex(int i) const override { return static_cast<StorageFunction *>(StorageExpressionModelStore::modelAtIndex(i)); }
  virtual StorageFunction * definedModelAtIndex(int i) const override { return static_cast<StorageFunction *>(StorageExpressionModelStore::definedModelAtIndex(i)); }
  virtual StorageFunction * activeFunctionAtIndex(int i) const;

  virtual char symbol() const = 0;
};

}

#endif
