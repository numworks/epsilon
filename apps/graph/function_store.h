#ifndef GRAPH_FUNCTION_STORE_H
#define GRAPH_FUNCTION_STORE_H

#include "function.h"

namespace Graph {

class FunctionStore {
public:
  FunctionStore();
  Function * functionAtIndex(int i);
  void pushFunction(const char * functionText);
  int numberOfFunctions();
private:
  int m_numberOfFunctions;
  static constexpr int k_maxNumberOfFunctions = 8;
  Function m_functions[k_maxNumberOfFunctions];
};

}

#endif
