#ifndef GRAPH_CONTINUOUS_FUNCTION_STORE_H
#define GRAPH_CONTINUOUS_FUNCTION_STORE_H

#include "../shared/cartesian_function.h"
#include "../shared/function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class ContinuousFunctionStore : public Shared::FunctionStore {
public:
  Shared::ExpiringPointer<Shared::ContinuousFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Shared::ContinuousFunction>(static_cast<Shared::ContinuousFunction *>(privateModelForRecord(record))); }
  int numberOfActiveFunctionsOfType(Shared::ContinuousFunction::PlotType plotType) const;
  Ion::Storage::Record activeRecordOfTypeAtIndex(Shared::ContinuousFunction::PlotType plotType, int index) const;
private:
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  Shared::ExpressionModelHandle * setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Shared::ContinuousFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
