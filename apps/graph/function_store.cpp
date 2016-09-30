#include "function_store.h"
extern "C" {
#include <assert.h>
}

constexpr int Graph::FunctionStore::k_numberOfDefaultColors;
constexpr KDColor Graph::FunctionStore::defaultColors[k_numberOfDefaultColors];
constexpr const char * Graph::FunctionStore::functionNames[k_maxNumberOfFunctions];

Graph::FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
}

Graph::Function * Graph::FunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

Graph::Function * Graph::FunctionStore::addEmptyFunction() {
  assert(m_numberOfFunctions < k_maxNumberOfFunctions);
  const char * name = firstAvailableName();
  KDColor color = firstAvailableColor();
  Graph::Function addedFunction = Function(name, color);
  m_functions[m_numberOfFunctions] = addedFunction;
  Function * result = &m_functions[m_numberOfFunctions];
  m_numberOfFunctions++;
  return result;
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

const char *  Graph::FunctionStore::firstAvailableName() {
  for (int k = 0; k < k_maxNumberOfFunctions; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].name() == functionNames[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return functionNames[k];
    }
  }
  return functionNames[0];
}

const KDColor  Graph::FunctionStore::firstAvailableColor() {
  for (int k = 0; k < k_numberOfDefaultColors; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].color() == defaultColors[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return defaultColors[k];
    }
  }
  return defaultColors[0];
}
