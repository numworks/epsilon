#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H

#include "../shared/storage_cartesian_function.h"
#include "../shared/storage_function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class StorageCartesianFunctionStore : public Shared::StorageFunctionStore {
public:
  StorageCartesianFunctionStore();
  Shared::StorageCartesianFunction * modelAtIndex(int i) const override { return static_cast<Shared::StorageCartesianFunction *>(Shared::StorageFunctionStore::modelAtIndex(i)); }
  Shared::StorageCartesianFunction * activeFunctionAtIndex(int i) const override { return static_cast<Shared::StorageCartesianFunction *>(Shared::StorageFunctionStore::activeFunctionAtIndex(i)); }
  Shared::StorageCartesianFunction * definedModelAtIndex(int i) const override { return static_cast<Shared::StorageCartesianFunction *>(Shared::StorageFunctionStore::definedModelAtIndex(i)); }
  char symbol() const override { return 'x'; }
  void removeAll() override;
private:
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  const char * modelExtension() const override { return Shared::GlobalContext::funcExtension; }
  void privateSetMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  void moveMemoizedModel(int previousIndex, int nextIndex) const override;
  Shared::StorageExpressionModel * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Shared::StorageCartesianFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
