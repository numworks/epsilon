#include "calcul_store.h"
#include <assert.h>

namespace Calcul {

CalculStore::CalculStore() :
  m_numberOfCalculs(0)
{
}

Calcul * CalculStore::calculAtIndex(int i) {
  assert(i>=0 && i<m_numberOfCalculs);
  return &m_calculs[i];
}

Calcul * CalculStore::addEmptyCalcul() {
  assert(m_numberOfCalculs < k_maxNumberOfCalculs);
  Calcul addedCalcul = Calcul();
  m_calculs[m_numberOfCalculs] = addedCalcul;
  Calcul * result = &m_calculs[m_numberOfCalculs];
  m_numberOfCalculs++;
  return result;
}

void CalculStore::removeCalcul(Calcul * c) {
  int i = 0;
  while (&m_calculs[i] != c && i < m_numberOfCalculs) {
    i++;
  }
  assert(i>=0 && i<m_numberOfCalculs);
  m_numberOfCalculs--;
  for (int j = i; j<m_numberOfCalculs; j++) {
    m_calculs[j] = m_calculs[j+1];
  }
}

int CalculStore::numberOfCalculs() {
  return m_numberOfCalculs;
}

}
