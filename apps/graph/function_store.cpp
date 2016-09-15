#include "function_store.h"
extern "C" {
#include <assert.h>
}

constexpr int numberOfDefaultColors = 3;
constexpr KDColor defaultColors[numberOfDefaultColors] = {
  KDColorRed, KDColorGreen, KDColorBlue
};

Graph::FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
}

Graph::Function * Graph::FunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

void Graph::FunctionStore::pushFunction(const char * functionText) {
  assert(m_numberOfFunctions < k_maxNumberOfFunctions);
  m_functions[m_numberOfFunctions] = Function(functionText, defaultColors[m_numberOfFunctions%numberOfDefaultColors]);
  m_numberOfFunctions++;
}

int Graph::FunctionStore::numberOfFunctions() {
  return m_numberOfFunctions;
}
