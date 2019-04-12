#ifndef GRAPH_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_CARTESIAN_FUNCTION_STORE_H

#include "../shared/cartesian_function.h"
#include "../shared/function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class CartesianFunctionStore : public Shared::FunctionStore {
public:
  Shared::ExpiringPointer<Shared::CartesianFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Shared::CartesianFunction>(static_cast<Shared::CartesianFunction *>(privateModelForRecord(record))); }
  CodePoint symbol() const override { return Shared::CartesianFunction::Symbol(); }
  CodePoint unknownSymbol() const override { return UCodePointUnknownX; }
private:
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Shared::CartesianFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
