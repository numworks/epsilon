#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "function.h"
#include "../shared/function_store.h"
#include <stdint.h>

namespace Graph {

class FunctionStore : public Shared::FunctionStore {
public:
  FunctionStore();
  uint32_t storeChecksum() override;
  Function * functionAtIndex(int i) override;
  Function * activeFunctionAtIndex(int i) override;
  Function * definedFunctionAtIndex(int i) override;
  Function * addEmptyFunction() override;
  void removeFunction(Shared::Function * f) override;
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
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
