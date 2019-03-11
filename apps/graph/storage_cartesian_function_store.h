#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H

#include "../shared/cartesian_function.h"
#include "../shared/storage_function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class StorageCartesianFunctionStore : public Shared::FunctionStore {
public:
  Shared::ExpiringPointer<Shared::CartesianFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Shared::CartesianFunction>(static_cast<Shared::CartesianFunction *>(privateModelForRecord(record))); }
  char symbol() const override { return Shared::CartesianFunction::Symbol(); }
  char unknownSymbol() const override { return Poincare::Symbol::SpecialSymbols::UnknownX; }
private:
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  const char * modelExtension() const override { return Ion::Storage::funcExtension; }
  void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::ExpressionModelHandle * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Shared::CartesianFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
