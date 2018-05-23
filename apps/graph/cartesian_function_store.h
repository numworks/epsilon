#ifndef GRAPH_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_CARTESIAN_FUNCTION_STORE_H

#include "cartesian_function.h"
#include "../shared/function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class CartesianFunctionStore : public Shared::FunctionStore {
public:
  CartesianFunctionStore();
  uint32_t storeChecksum() override;
  CartesianFunction * modelAtIndex(int i) override { return &m_functions[i]; }
  CartesianFunction * activeFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::activeFunctionAtIndex(i); }
  CartesianFunction * definedFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::definedFunctionAtIndex(i); }
  int maxNumberOfModels() const override {
    return k_maxNumberOfFunctions;
  }
  char symbol() const override;
  void removeAll() override;
  static constexpr int k_maxNumberOfFunctions = 4;
private:
  static constexpr KDColor k_defaultColors[k_maxNumberOfFunctions] = {
    Palette::Red, Palette::Blue,  Palette::Green, Palette::YellowDark,
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p",
  };
  Shared::Function * emptyModel() override;
  Shared::Function * nullModel() override;
  void setModelAtIndex(Shared::Function * f, int i) override;
  const char * firstAvailableName() override {
    return firstAvailableAttribute(k_functionNames, FunctionStore::name);
  }
  const KDColor firstAvailableColor() override {
    return firstAvailableAttribute(k_defaultColors, FunctionStore::color);
  }
  CartesianFunction m_functions[k_maxNumberOfFunctions];
};

}

#endif
