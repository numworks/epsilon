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

void ExpressionModelStore::tidy() {
  for (int i = 0; i < m_numberOfModels; i++) {
    modelAtIndex(i)->tidy();
  }
}

}
