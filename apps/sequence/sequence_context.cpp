#include "sequence_context.h"
#include "sequence_store.h"
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Sequence {

template<typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext() :
  m_rank(-1),
  m_values{{NAN, NAN, NAN},{NAN, NAN, NAN}}
{
}

template<typename T>
T TemplatedSequenceContext<T>::valueOfSequenceAtPreviousRank(int sequenceIndex, int rank) const {
  return m_values[sequenceIndex][rank];
}

template<typename T>
void TemplatedSequenceContext<T>::resetCache() {
  m_rank = -1;
}

template<typename T>
bool TemplatedSequenceContext<T>::iterateUntilRank(int n, SequenceStore * sequenceStore, SequenceContext * sqctx) {
  if (m_rank > n) {
    m_rank = -1;
  }
  if (n < 0 || n-m_rank > k_maxRecurrentRank) {
    return false;
  }
  while (m_rank++ < n) {
    step(sequenceStore, sqctx);
  }
  m_rank--;
  return true;
}

template<typename T>
void TemplatedSequenceContext<T>::step(SequenceStore * sequenceStore, SequenceContext * sqctx) {
  /* Shift values */
  for (int i = 0; i < MaxNumberOfSequences; i++) {
    for (int j = MaxRecurrenceDepth; j > 0; j--) {
      m_values[i][j] = m_values[i][j-1];
    }
    m_values[i][0] = NAN;
  }

  /* Evaluate new u(n) and v(n) */
  ExpiringPointer<Sequence> u = sequenceStore->numberOfModels() > 0 ? sequenceStore->modelForRecord(sequenceStore->recordAtIndex(0)) : nullptr;
  u = u.isNull() && u->isDefined() ? u : nullptr;
  ExpiringPointer<Sequence> v = sequenceStore->numberOfModels() > 1 ? sequenceStore->modelForRecord(sequenceStore->recordAtIndex(1)) : nullptr;
  v = v.isNull() && v->isDefined() ? v : nullptr;
  /* Switch u & v  if the name of u is v */
  if (!u.isNull() && u->fullName()[0] ==  SequenceStore::k_sequenceNames[1][0]) {
     ExpiringPointer<Sequence> temp = u;
    u = v;
    v = temp;
  }

  /* Approximate u & v at the new rank. We evaluate u twice in case its
   * expression depends on v. */
  m_values[0][0] = !u.isNull() ? u->approximateToNextRank<T>(m_rank, sqctx) : NAN;
  m_values[1][0] = !v.isNull() ? v->approximateToNextRank<T>(m_rank, sqctx) : NAN;
  m_values[0][0] = !u.isNull() ? u->approximateToNextRank<T>(m_rank, sqctx) : NAN;
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;

}
