#include "sequence_context.h"
#include "sequence_store.h"
#include "sequence_cache_context.h"
#include "../shared/poincare_helpers.h"
#include <cmath>

using namespace Poincare;

namespace Shared {

template<typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext() :
  m_commonRank(-1),
  m_commonRankValues{{NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}},
  m_independentRanks{-1, -1, -1},
  m_independentRankValues{{NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}}
{
}

template<typename T>
T TemplatedSequenceContext<T>::valueOfCommonRankSequenceAtPreviousRank(int sequenceIndex, int rank) const {
  return m_commonRankValues[sequenceIndex][rank];
}

template<typename T>
void TemplatedSequenceContext<T>::resetCache() {
  /* We only need to reset the ranks. Indeed, when we compute the values of the
   * sequences, we use ranks as memoization indexes. Therefore, we know that the
   * values stored in m_commomValues and m_independentRankValues are dirty
   * and do not use them. */
  m_commonRank = -1;
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i ++) {
    m_independentRanks[i] = -1;
  }
}

template<typename T>
bool TemplatedSequenceContext<T>::iterateUntilRank(int n, SequenceStore * sequenceStore, SequenceContext * sqctx) {
  if (m_commonRank > n) {
    m_commonRank = -1;
  }
  if (n < 0 || n-m_commonRank > k_maxRecurrentRank) {
    return false;
  }
  while (m_commonRank < n) {
    step(sqctx);
  }
  return true;
}

template<typename T>
void TemplatedSequenceContext<T>::step(SequenceContext * sqctx, int sequenceIndex) {
  // First we increment the rank
  bool stepMultipleSequences = sequenceIndex == -1;
  if (stepMultipleSequences) {
    m_commonRank++;
  } else {
    setIndependentSequenceRank(independentSequenceRank(sequenceIndex) + 1, sequenceIndex);
  }

  // Then we shift the values stored in the arrays
  int start, stop, rank;
  T * sequenceArray;
  if (stepMultipleSequences) {
    start = 0;
    stop = SequenceStore::k_maxNumberOfSequences;
    sequenceArray = reinterpret_cast<T*>(&m_commonRankValues);
    rank = m_commonRank;
  } else {
    start = sequenceIndex;
    stop = sequenceIndex + 1;
    sequenceArray = reinterpret_cast<T*>(&m_independentRankValues);
    rank = independentSequenceRank(sequenceIndex);
  }

  for (int i = start; i < stop; i++) {
    T * rowPointer = sequenceArray + i * (SequenceStore::k_maxRecurrenceDepth + 1);
    for (int j = SequenceStore::k_maxRecurrenceDepth; j > 0; j--) {
      *(rowPointer + j) = *(rowPointer + j - 1);
    }
    *rowPointer= NAN;
  }

  // We create an array containing the sequences we want to update
  Sequence * sequences[SequenceStore::k_maxNumberOfSequences] = {nullptr, nullptr, nullptr};
  int usedSize = stepMultipleSequences ? SequenceStore::k_maxNumberOfSequences : 1;
  SequenceStore * sequenceStore = sqctx->sequenceStore();
  for (int i = start; i < stop; i++) {
    Ion::Storage::Record record = sequenceStore->recordAtNameIndex(i);
    if (!record.isNull()) {
      Sequence * u = sequenceStore->modelForRecord(record);
      int index = stepMultipleSequences ? SequenceStore::sequenceIndexForName(u->fullName()[0]) : 0;
      sequences[index] = u->isDefined() ? u : nullptr;
    }
  }

  // We approximate the value of the next rank for each sequence we want to update
  stop = stepMultipleSequences ? SequenceStore::k_maxNumberOfSequences : sequenceIndex + 1;
  /* In case stop is SequenceStore::k_maxNumberOfSequences, we approximate u, v and w at the new
   * rank. We have to evaluate all sequences SequenceStore::k_maxNumberOfSequences times in case
   * the evaluations depend on each other.
   * For example, if u expression depends on v and v depends on w. At the first
   * iteration, we can only evaluate w, at the second iteration we evaluate v
   * and u can only be known at the third iteration.
   * In case stop is 1, there is only one sequence we want to update. Moreover,
   * the call to approximateToNextRank will handle potential dependencies. */
  for (int k = 0; k < usedSize; k++) {
    for (int i = start; i < stop; i++) {
      T * pointerToUpdate = sequenceArray + i * (SequenceStore::k_maxRecurrenceDepth + 1);
      if (std::isnan(*(pointerToUpdate))) {
        int j = stepMultipleSequences ? i : 0;
        *pointerToUpdate = sequences[j] ? sequences[j]->template approximateToNextRank<T>(rank, sqctx, sequenceIndex) : NAN;
      }
    }
  }
}

void SequenceContext::tidyDownstreamPoolFrom(char * treePoolCursor) {
  m_sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;
template void * SequenceContext::helper<float>();
template void * SequenceContext::helper<double>();

}
