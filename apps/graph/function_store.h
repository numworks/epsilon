#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "function.h"

namespace Graph {
/* FunctionStore is a dumb class.
 * Its only job is to store functions and to give them a color. */
class FunctionStore {
public:
  FunctionStore();
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
  static constexpr int k_numberOfDefaultColors = 10;
  static constexpr KDColor k_defaultColors[k_numberOfDefaultColors] = {
    KDColor::RGB24(0xE5473E), KDColor::RGB24(0x57B1D9), KDColor::RGB24(0xE49832), KDColor::RGB24(0x6DBE51),
    KDColor::RGB24(0x199B99), KDColor::RGB24(0xF2A37C), KDColor::RGB24(0x37C995), KDColor::RGB24(0x6759D0),
    KDColor::RGB24(0xEF7138), KDColor::RGB24(0x1774C5)
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p", "q", "r", "s", "t"
  };
  int m_numberOfFunctions;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
