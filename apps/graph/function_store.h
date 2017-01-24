#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "function.h"
#include <stdint.h>

namespace Graph {
/* FunctionStore is a dumb class.
 * Its only job is to store functions and to give them a color. */
class FunctionStore {
public:
  FunctionStore();
  uint32_t storeChecksum();
  Function * functionAtIndex(int i);
  Function * activeFunctionAtIndex(int i);
  Function * definedFunctionAtIndex(int i);
  Function * addEmptyFunction();
  void removeFunction(Function * f);
  int numberOfFunctions();
  // Functions can be undefined when they have a color and a name but no content
  int numberOfDefinedFunctions();
  // An active function must be defined to be counted
  int numberOfActiveFunctions();
  static constexpr int k_maxNumberOfFunctions = 8;
private:
  const char * firstAvailableName();
  const KDColor firstAvailableColor();
  static constexpr int k_numberOfDefaultColors = 8;
  static constexpr KDColor k_defaultColors[k_numberOfDefaultColors] = {
    KDColor::RGB24(0xbe2727), KDColor::RGB24(0x3e6f3c), KDColor::RGB24(0x656975), KDColor::RGB24(0x9ec580),
    KDColor::RGB24(0xffb734), KDColor::RGB24(0x4a94ab), KDColor::RGB24(0xf98577), KDColor::RGB24(0xd4ac46)
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p", "q", "r", "s", "t"
  };
  int m_numberOfFunctions;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
