#include "function_store.h"
extern "C" {
#include <assert.h>
}

constexpr int Graph::FunctionStore::numberOfDefaultColors;
constexpr KDColor Graph::FunctionStore::defaultColors[numberOfDefaultColors];

Graph::FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
}

Graph::Function * Graph::FunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

void Graph::FunctionStore::addFunction(Function * f) {
  assert(m_numberOfFunctions < k_maxNumberOfFunctions);
  m_functions[m_numberOfFunctions++] = (*f);
}

void Graph::FunctionStore::removeFunction(Function * f) {
  int i = 0;
  while (&m_functions[i] != f && i < m_numberOfFunctions) {
    i++;
  }
  assert(i>=0 && i<m_numberOfFunctions);
  m_numberOfFunctions--;
  for (int j = i; j<m_numberOfFunctions; j++) {
    m_functions[j] = m_functions[j+1];
  }
}

int Graph::FunctionStore::numberOfFunctions() {
  return m_numberOfFunctions;
}
