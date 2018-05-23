#include "model_store.h"
#include "function.h"
#include <assert.h>

namespace Shared {

template<typename T>
ModelStore<T>::ModelStore() :
  m_numberOfModels(0)
{
}

template<typename T>
T * ModelStore<T>::addEmptyModel() {
  assert(m_numberOfModels < maxNumberOfModels());
  setModelAtIndex(emptyModel(), m_numberOfModels++);
  return modelAtIndex(m_numberOfModels-1);
}

template<typename T>
void ModelStore<T>::removeModel(T * f) {
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

template<typename T>
void ModelStore<T>::removeAll() {
  for (int i = 0; i < m_numberOfModels; i++) {
    setModelAtIndex(nullModel(), i);
  }
  m_numberOfModels = 0;
}

template<typename T>
void ModelStore<T>::tidy() {
  for (int i = 0; i < m_numberOfModels; i++) {
    tidyModelAtIndex(i);
  }
}

}

template class Shared::ModelStore<Shared::Function>;
