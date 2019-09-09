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
  uint32_t storeChecksum();
  // An active function must be defined to be counted
  int numberOfActiveFunctions() const { return numberOfModelsSatisfyingTest([](ExpressionModelHandle * m) { return m->isDefined() && static_cast<Function *>(m)->isActive(); }); }
  Ion::Storage::Record activeRecordAtIndex(int i) const { return recordSatisfyingTestAtIndex(i, [](ExpressionModelHandle * m) { return m->isDefined() && static_cast<Function *>(m)->isActive(); }); }

  ExpiringPointer<Function> modelForRecord(Ion::Storage::Record record) const { return ExpiringPointer<Function>(static_cast<Function *>(privateModelForRecord(record))); }

};

}

#endif
