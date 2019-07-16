#include "sequence_context.h"
#include "sequence_store.h"
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Sequence {

template<typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext() :
  m_rank(-1),
  m_values{{NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}}
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
  // sequences hold u, v, w in this order
  Sequence * sequences[MaxNumberOfSequences] = {nullptr, nullptr, nullptr};
  for (int i = 0; i < sequenceStore->numberOfModels(); i++) {
    Sequence * u = sequenceStore->modelForRecord(sequenceStore->recordAtIndex(i));
    sequences[SequenceStore::sequenceIndexForName(u->fullName()[0])] = u->isDefined() ? u : nullptr;
  }

  /* Approximate u, v and w at the new rank. We have to evaluate all sequences
   * MaxNumberOfSequences times in case the evaluations depend on each other.
   * For example, if u expression depends on v and v depends on w. At the first
   * iteration, we can only evaluate w, at the second iteration we evaluate v
   * and u can only be known at the third iteration . */
  for (int k = 0; k < MaxNumberOfSequences; k++) {
    for (int i = 0; i < MaxNumberOfSequences; i++) {
      if (std::isnan(m_values[i][0])) {
        m_values[i][0] = sequences[i] ? sequences[i]->approximateToNextRank<T>(m_rank, sqctx) : NAN;
      }
    }
  }
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;

}
