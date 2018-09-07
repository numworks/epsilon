#include "expression_model_store.h"
#include "function.h"
#include <assert.h>

namespace Shared {

ExpressionModelStore::ExpressionModelStore() :
  m_numberOfModels(0)
{
}

ExpressionModel * ExpressionModelStore::addEmptyModel() {
  assert(m_numberOfModels < maxNumberOfModels());
  setModelAtIndex(emptyModel(), m_numberOfModels++);
  return modelAtIndex(m_numberOfModels-1);
}

void ExpressionModelStore::removeModel(ExpressionModel * f) {
  int i = 0;
  while (modelAtIndex(i) != f && i < m_numberOfModels) {
    i++;
  }
  assert(i>=0 && i<m_numberOfModels);
  for (int j = i; j<m_numberOfModels-1; j++) {
    setModelAtIndex(modelAtIndex(j+1), j);
  }
  setModelAtIndex(nullModel(), m_numberOfModels-1);
  m_numberOfModels--;
}

void ExpressionModelStore::removeAll() {
  for (int i = 0; i < m_numberOfModels; i++) {
    setModelAtIndex(nullModel(), i);
  }
  m_numberOfModels = 0;
}

ExpressionModel * ExpressionModelStore::definedModelAtIndex(int i) {
  assert(i>=0 && i<m_numberOfModels);
  int index = 0;
  for (int k = 0; k < m_numberOfModels; k++) {
    if (modelAtIndex(k)->isDefined()) {
      if (i == index) {
        return modelAtIndex(k);
      }
      index++;
    }
  }
  assert(false);
  return nullptr;
}

int ExpressionModelStore::numberOfDefinedModels() {
  int result = 0;
  for (int i = 0; i < m_numberOfModels; i++) {
    if (modelAtIndex(i)->isDefined()) {
      result++;
    }
  }
  return result;
}

void ExpressionModelStore::tidy() {
  for (int i = 0; i < m_numberOfModels; i++) {
    modelAtIndex(i)->tidy();
  }
}

}
