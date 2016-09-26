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
  constexpr static int numberOfDefaultColors = 4;
  constexpr static KDColor defaultColors[numberOfDefaultColors] = {
    KDColorRed, KDColorGreen, KDColorBlue, KDColorBlack
  };
  int m_numberOfFunctions;
  static constexpr int k_maxNumberOfFunctions = 8;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
