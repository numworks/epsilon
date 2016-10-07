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
  Function * addEmptyFunction();
  void removeFunction(Function * f);
  int numberOfFunctions();

private:
  const char * firstAvailableName();
  const KDColor firstAvailableColor();
  static constexpr int k_maxNumberOfFunctions = 8;
  static constexpr int k_numberOfDefaultColors = 8;
  static constexpr KDColor k_defaultColors[k_numberOfDefaultColors] = {
    KDColorRed, KDColorGreen, KDColorBlue, KDColorBlack,
    KDColor(0xBFD3EB), KDColor(0xFF00FF), KDColor(0x00FFEB), KDColor(0xBFAA00)
  };
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "i", "j", "k", "l", "m"
  };
  int m_numberOfFunctions;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
