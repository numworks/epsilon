#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "function.h"

namespace Graph {

class FunctionStore {
public:
  FunctionStore();
  Function * functionAtIndex(int i);
  void addFunction(Function * f);
  void removeFunction(Function * f);
  void pushFunction(const char * functionText);

  int numberOfFunctions();

  constexpr static int numberOfDefaultColors = 3;
  constexpr static KDColor defaultColors[numberOfDefaultColors] = {
    KDColorRed, KDColorGreen, KDColorBlue
  };
private:
  int m_numberOfFunctions;
  static constexpr int k_maxNumberOfFunctions = 8;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
