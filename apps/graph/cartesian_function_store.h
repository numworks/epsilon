#ifndef GRAPH_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_CARTESIAN_FUNCTION_STORE_H

#include "cartesian_function.h"
#include "../shared/function_store.h"
#include <stdint.h>

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
  static constexpr int k_maxNumberOfFunctions = 8;
private:
  const char * firstAvailableName() override;
  const KDColor firstAvailableColor() override;
  static constexpr int k_numberOfDefaultColors = 8;
  static constexpr KDColor k_defaultColors[k_numberOfDefaultColors] = {
    KDColor::RGB24(0xbe2727), KDColor::RGB24(0x3e6f3c), KDColor::RGB24(0x656975), KDColor::RGB24(0x9ec580),
    KDColor::RGB24(0xffb734), KDColor::RGB24(0x4a94ab), KDColor::RGB24(0xf98577), KDColor::RGB24(0xd4ac46)
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p", "q", "r", "s", "t"
  };
  CartesianFunction m_functions[k_maxNumberOfFunctions];
};

}

#endif
