#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H

#include "../shared/storage_cartesian_function.h"
#include "../shared/storage_function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class StorageCartesianFunctionStore : public Shared::StorageFunctionStore<Shared::StorageCartesianFunction> {
public:
  static Shared::StorageCartesianFunction NullModel();

  StorageCartesianFunctionStore();
/*  StorageCartesianFunction activeCartesianFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::activeFunctionAtIndex(i); }
  StorageCartesianFunction definedFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::definedFunctionAtIndex(i); }
*/
  char symbol() const override;
  void removeAll() override;
  static constexpr int k_maxNumberOfFunctions = 4; //TODO
private:
/*  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p" //TODO
  };

  const char * firstAvailableName() override {
    return firstAvailableAttribute(k_functionNames, FunctionStore::name);
  }*/
  const char * firstAvailableName() override { return "f"; }
};

}

#endif
