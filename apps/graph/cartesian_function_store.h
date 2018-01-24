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
  CartesianFunction * functionAtIndex(int i) override;
  CartesianFunction * activeFunctionAtIndex(int i) override;
  CartesianFunction * definedFunctionAtIndex(int i) override;
  CartesianFunction * addEmptyFunction() override;
  void removeFunction(Shared::Function * f) override;
  int maxNumberOfFunctions() override;
  char symbol() const override;
  void removeAll() override;
  static constexpr int k_maxNumberOfFunctions = 4;
private:
  const char * firstAvailableName() override;
  const KDColor firstAvailableColor() override;
  static constexpr KDColor k_defaultColors[k_maxNumberOfFunctions] = {
    Palette::Red, Palette::Blue,  Palette::Green, Palette::YellowDark,
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p",
  };
  CartesianFunction m_functions[k_maxNumberOfFunctions];
};

}

#endif
