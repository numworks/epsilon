#include "sequence_context.h"

#include <array>
#include <cmath>

#include "../shared/poincare_helpers.h"
#include "intermediate_sequence_context.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

template <typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext(
    SequenceContext *sequenceContext)
    : m_sequenceContext(sequenceContext),
      m_commonRank(-1),
      m_commonRankValues{{NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}},
      m_independentRanks{-1, -1, -1},
      m_independentRankValues{
          {NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}} {}

template <typename T>
void TemplatedSequenceContext<T>::resetCacheOfSequence(int sequenceIndex) {
  int start, stop;
  T *sequencesRankValues;
  if (sequenceIndex != -1) {
    m_independentRanks[sequenceIndex] = -1;
    start = sequenceIndex;
    stop = sequenceIndex + 1;
    sequencesRankValues = reinterpret_cast<T *>(&m_independentRankValues);
  } else {
    m_commonRank = -1;
    start = 0;
    stop = SequenceStore::k_maxNumberOfSequences;
    sequencesRankValues = reinterpret_cast<T *>(&m_commonRankValues);
  }
  for (int sequence = start; sequence < stop; sequence++) {
    T *sequencePointer = sequencesRankValues +
                         sequence * (SequenceStore::k_maxRecurrenceDepth + 1);
    for (int depth = 0; depth < SequenceStore::k_maxRecurrenceDepth + 1;
         depth++) {
      *(sequencePointer + depth) = static_cast<T>(NAN);
    }
  }
}

template <typename T>
void TemplatedSequenceContext<T>::resetCache() {
  for (int i = -1; i < SequenceStore::k_maxNumberOfSequences; i++) {
    resetCacheOfSequence(i);
  }
}

template <typename T>
T TemplatedSequenceContext<T>::storedValueOfSequenceAtRank(int sequenceIndex,
                                                           int rank) {
  if (m_commonRank >= 0) {
    int offset = m_commonRank - rank;
    if (0 <= offset && offset < SequenceStore::k_maxRecurrenceDepth + 1 &&
        !std::isnan(m_commonRankValues[sequenceIndex][offset])) {
      return m_commonRankValues[sequenceIndex][offset];
    }
  }
  if (m_independentRanks[sequenceIndex] >= 0) {
    int offset = m_independentRanks[sequenceIndex] - rank;
    if (0 <= offset && offset < SequenceStore::k_maxRecurrenceDepth + 1 &&
        !std::isnan(m_independentRankValues[sequenceIndex][offset])) {
      return m_independentRankValues[sequenceIndex][offset];
    }
  }
  return static_cast<T>(NAN);
}

template <typename T>
void TemplatedSequenceContext<T>::stepUntilRank(int n, int sequenceIndex) {
  assert(IsAcceptableRank(n));
  bool independent = sequenceIndex != -1;
  int *currentRank =
      independent ? m_independentRanks + sequenceIndex : &m_commonRank;
  assert(*currentRank >= 0 || *currentRank == -1);

  // If current rank is superior to n, we need to start computing back the
  // recurrence from the initial rank and step until rank n. Otherwise, we can
  // start at the current rank and step until rank n.
  if (*currentRank > n) {
    resetCacheOfSequence(sequenceIndex);
  }
  while (*currentRank < n) {
    stepToNextRank(sequenceIndex);
  }
}

template <typename T>
void TemplatedSequenceContext<T>::stepToNextRank(int sequenceIndex) {
  // First we increment the rank
  bool independent = sequenceIndex != -1;
  int *currentRank =
      independent ? m_independentRanks + sequenceIndex : &m_commonRank;
  assert(*currentRank >= 0 || *currentRank == -1);
  (*currentRank)++;

  // Then we shift the values stored in the arrays
  int start, stop;
  T *sequencesRankValues;
  if (independent) {
    start = sequenceIndex;
    stop = sequenceIndex + 1;
    sequencesRankValues = reinterpret_cast<T *>(&m_independentRankValues);
  } else {
    start = 0;
    stop = SequenceStore::k_maxNumberOfSequences;
    sequencesRankValues = reinterpret_cast<T *>(&m_commonRankValues);
  }
  if (*currentRank > 0) {
    // If rank was -1, all values are NAN, then no need to shift
    for (int sequence = start; sequence < stop; sequence++) {
      T *sequencePointer = sequencesRankValues +
                           sequence * (SequenceStore::k_maxRecurrenceDepth + 1);
      // {u(n), u(n-1), u(n-2)} becomes {NaN, u(n), u(n-1)}
      for (int depth = SequenceStore::k_maxRecurrenceDepth; depth > 0;
           depth--) {
        *(sequencePointer + depth) = *(sequencePointer + depth - 1);
      }
      *sequencePointer = NAN;
    }
  }

  // We create an array containing the sequences we want to update
  Sequence *sequencesToUpdate[SequenceStore::k_maxNumberOfSequences] = {
      nullptr, nullptr, nullptr};
  SequenceStore *sequenceStore = m_sequenceContext->sequenceStore();
  for (int sequence = start; sequence < stop; sequence++) {
    Ion::Storage::Record record = sequenceStore->recordAtNameIndex(sequence);
    if (record.isNull()) {
      continue;
    }
    Sequence *s = sequenceStore->modelForRecord(record);
    if (!s->isDefined()) {
      continue;
    }
    /* WARNING : values must be ordered by name index (u then v then w) because
     * approximateAtContextRank needs it. */
    int index = SequenceStore::SequenceIndexForName(s->fullName()[0]);
    sequencesToUpdate[index] = s;
  }

  // We approximate the value of the next rank for each sequence we want to
  // update
  for (int j = start; j < stop; j++) {
    if (!sequencesToUpdate[j]) {
      continue;
    }
    T *sequencePointer =
        sequencesRankValues + j * (SequenceStore::k_maxRecurrenceDepth + 1);
    if (std::isnan(*sequencePointer)) {
      T otherCacheValue = storedValueOfSequenceAtRank(j, *currentRank);
      if (!std::isnan(otherCacheValue)) {
        // If the other cache already knows this value, use it
        *sequencePointer = otherCacheValue;
      } else {
        *sequencePointer = sequencesToUpdate[j]->approximateAtContextRank<T>(
            m_sequenceContext, independent);
      }
    }
  }
}

void SequenceContext::tidyDownstreamPoolFrom(char *treePoolCursor) {
  m_sequenceStore->tidyDownstreamPoolFrom(treePoolCursor);
}

Poincare::Context::SymbolAbstractType
SequenceContext::expressionTypeForIdentifier(const char *identifier,
                                             int length) {
  constexpr int numberOfSequencesNames =
      std::size(SequenceStore::k_sequenceNames);
  for (int i = 0; i < numberOfSequencesNames; i++) {
    if (strncmp(identifier, SequenceStore::k_sequenceNames[i], length) == 0) {
      return Poincare::Context::SymbolAbstractType::Sequence;
    }
  }
  return Poincare::ContextWithParent::expressionTypeForIdentifier(identifier,
                                                                  length);
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;
template void *SequenceContext::helper<float>();
template void *SequenceContext::helper<double>();

}  // namespace Shared
