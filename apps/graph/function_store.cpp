#include "function_store.h"
extern "C" {
#include <assert.h>
}

constexpr int Graph::FunctionStore::k_numberOfDefaultColors;
constexpr KDColor Graph::FunctionStore::k_defaultColors[k_numberOfDefaultColors];
constexpr const char * Graph::FunctionStore::k_functionNames[k_maxNumberOfFunctions];

Graph::FunctionStore::FunctionStore() :
  m_numberOfFunctions(0)
{
}

Graph::Function * Graph::FunctionStore::functionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  return &m_functions[i];
}

Graph::Function * Graph::FunctionStore::activeFunctionAtIndex(int i) {
  assert(i>=0 && i<m_numberOfFunctions);
  int index = 0;
  for (int k = 0; k < m_numberOfFunctions; k++) {
    if (m_functions[k].isActive()) {
      if (i == index) {
        return &m_functions[k];
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
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

int Graph::FunctionStore::numberOfActiveFunctions() {
  int result = 0;
  for (int i = 0; i < m_numberOfFunctions; i++) {
    if (m_functions[i].isActive()) {
      result++;
    }
  }
  return result;
}

const char *  Graph::FunctionStore::firstAvailableName() {
  for (int k = 0; k < k_maxNumberOfFunctions; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].name() == k_functionNames[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_functionNames[k];
    }
  }
  return k_functionNames[0];
}

const KDColor  Graph::FunctionStore::firstAvailableColor() {
  for (int k = 0; k < k_numberOfDefaultColors; k++) {
    int j = 0;
    while  (j < m_numberOfFunctions) {
      if (m_functions[j].color() == k_defaultColors[k]) {
        break;
      }
      j++;
    }
    if (j == m_numberOfFunctions) {
      return k_defaultColors[k];
    }
  }
  return k_defaultColors[0];
}
