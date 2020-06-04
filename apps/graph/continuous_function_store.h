#ifndef GRAPH_CONTINUOUS_FUNCTION_STORE_H
#define GRAPH_CONTINUOUS_FUNCTION_STORE_H

#include "../shared/function_store.h"
#include "../shared/continuous_function.h"

namespace Graph {

class ContinuousFunctionStore : public Shared::FunctionStore {
public:
  bool displaysNonCartesianFunctions(int * nbActiveFunctions = nullptr) const;
  int numberOfActiveFunctionsOfType(Shared::ContinuousFunction::PlotType plotType) const {
    return numberOfModelsSatisfyingTest(&isFunctionActiveOfType, &plotType);
  }
  Ion::Storage::Record activeRecordOfTypeAtIndex(Shared::ContinuousFunction::PlotType plotType, int i) const {
    return recordSatisfyingTestAtIndex(i, &isFunctionActiveOfType, &plotType);
  }
  Shared::ExpiringPointer<Shared::ContinuousFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Shared::ContinuousFunction>(static_cast<Shared::ContinuousFunction *>(privateModelForRecord(record))); }
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
private:
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  static bool isFunctionActiveOfType(Shared::ExpressionModelHandle * model, void * context) {
    Shared::ContinuousFunction::PlotType plotType = *static_cast<Shared::ContinuousFunction::PlotType *>(context);
    return isFunctionActive(model, context) && plotType == static_cast<Shared::ContinuousFunction *>(model)->plotType();
  }
  mutable Shared::ContinuousFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
