#ifndef SHARED_FUNCTION_STORE_H
#define SHARED_FUNCTION_STORE_H

#include "function.h"
#include "expression_model_store.h"
#include <stdint.h>

namespace Shared {

// FunctionStore stores functions and gives them a color.

class FunctionStore : public ExpressionModelStore {
public:
  FunctionStore() : ExpressionModelStore() {}
  int numberOfActiveFunctions() const {
    return numberOfModelsSatisfyingTest(&IsFunctionActive, nullptr);
  }
  Ion::Storage::Record activeRecordAtIndex(int i) const {
    return recordSatisfyingTestAtIndex(i, &IsFunctionActive, nullptr);
  }
  ExpiringPointer<Function> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<Function>(static_cast<Function *>(privateModelForRecord(record))); }
protected:
  static bool IsFunctionActive(ExpressionModelHandle * model, void * context) {
    // An active function must be defined
    return isModelDefined(model, context) && static_cast<Function *>(model)->isActive();
  }
};

}

#endif
