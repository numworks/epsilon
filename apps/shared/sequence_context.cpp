#include "sequence_context.h"

#include <omg/signaling_nan.h>

#include <array>
#include <cmath>

#include "../shared/poincare_helpers.h"
#include "sequence_store.h"

using namespace Poincare;

namespace Shared {

template <typename T>
TemplatedSequenceContext<T>::TemplatedSequenceContext(
    SequenceContext *sequenceContext)
    : ContextWithParent(sequenceContext), m_sequenceContext(sequenceContext) {
  resetCache();
}

template <typename T>
void TemplatedSequenceContext<T>::resetCacheOfSequence(
    int sequenceIndex, bool intermediateComputation) {
  T *values;
  if (intermediateComputation) {
    m_intermediateRanks[sequenceIndex] = -1;
    values = reinterpret_cast<T *>(&m_intermediateValues);
  } else {
    m_mainRanks[sequenceIndex] = -1;
    values = reinterpret_cast<T *>(&m_mainValues);
  }
  T *ptr = values + sequenceIndex * (SequenceStore::k_maxRecurrenceDepth + 1);
  for (int depth = 0; depth < SequenceStore::k_maxRecurrenceDepth + 1;
       depth++) {
    *(ptr + depth) = OMG::SignalingNan<T>();
  }
}

template <typename T>
void TemplatedSequenceContext<T>::resetDataOfCurrentComputation() {
  m_isComputingMainResult = false;
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    m_smallestRankBeingComputed[i] = -1;
  }
}

template <typename T>
void TemplatedSequenceContext<T>::resetCache() {
  for (int i = 0; i < SequenceStore::k_maxNumberOfSequences; i++) {
    resetCacheOfSequence(i, true);
    resetCacheOfSequence(i, false);
  }
  resetDataOfCurrentComputation();
}

template <typename T>
T TemplatedSequenceContext<T>::storedValueOfSequenceAtRank(int sequenceIndex,
                                                           int rank) {
  for (int loop = 0; loop < 2; loop++) {
    int storedRank = loop == 0 ? m_mainRanks[sequenceIndex]
                               : m_intermediateRanks[sequenceIndex];
    if (storedRank >= 0) {
      int offset = storedRank - rank;
      if (0 <= offset && offset < SequenceStore::k_maxRecurrenceDepth + 1) {
        T storedValue = loop == 0 ? m_mainValues[sequenceIndex][offset]
                                  : m_intermediateValues[sequenceIndex][offset];
        if (!OMG::IsSignalingNan(storedValue)) {
          return storedValue;
        }
      }
    }
  }
  return OMG::SignalingNan<T>();
}

template <typename T>
void TemplatedSequenceContext<T>::stepUntilRank(int sequenceIndex, int rank) {
  assert(IsAcceptableRank(rank));
  bool intermediateComputation = m_isComputingMainResult;
  m_isComputingMainResult = true;

  int *currentRank = intermediateComputation
                         ? m_intermediateRanks + sequenceIndex
                         : m_mainRanks + sequenceIndex;
  assert(*currentRank >= 0 || *currentRank == -1);

  // If current rank is superior to n, we need to start computing back the
  // recurrence from the initial rank and step until rank n. Otherwise, we can
  // start at the current rank and step until rank n.
  if (*currentRank > rank) {
    resetCacheOfSequence(sequenceIndex, intermediateComputation);
  }
  while (*currentRank < rank) {
    stepToNextRank(sequenceIndex, intermediateComputation);
  }
  if (!intermediateComputation) {
    resetDataOfCurrentComputation();
  }
}

template <typename T>
void TemplatedSequenceContext<T>::stepToNextRank(int sequenceIndex,
                                                 bool intermediateComputation) {
  int *currentRank;
  T *values;
  if (intermediateComputation) {
    currentRank = m_intermediateRanks;
    values = reinterpret_cast<T *>(&m_intermediateValues);
  } else {
    currentRank = m_mainRanks;
    values = reinterpret_cast<T *>(&m_mainValues);
  }
  currentRank += sequenceIndex;
  values += sequenceIndex * (SequenceStore::k_maxRecurrenceDepth + 1);

  // First we increment the rank
  (*currentRank)++;

  // Then we shift the values stored in the arrays : {u(n), u(n-1), u(n-2)}
  // becomes {SignalingNan, u(n), u(n-1)}. If rank was -1, all values are
  // SignalingNan, then no need to shift.
  if (*currentRank > 0) {
    for (int depth = SequenceStore::k_maxRecurrenceDepth; depth > 0; depth--) {
      *(values + depth) = *(values + depth - 1);
    }
    *values = OMG::SignalingNan<T>();
  }

  // We approximate the value at new rank
  SequenceStore *sequenceStore = m_sequenceContext->sequenceStore();
  Ion::Storage::Record record = sequenceStore->recordAtNameIndex(sequenceIndex);
  assert(!record.isNull());
  Sequence *s = sequenceStore->modelForRecord(record);
  assert(s->isDefined());
  T otherCacheValue = storedValueOfSequenceAtRank(sequenceIndex, *currentRank);
  if (!OMG::IsSignalingNan(otherCacheValue)) {
    // If the other cache already knows this value, use it
    *values = otherCacheValue;
  } else {
    int previousSmallestRank = m_smallestRankBeingComputed[sequenceIndex];
    if (previousSmallestRank == -1 || *currentRank < previousSmallestRank) {
      m_smallestRankBeingComputed[sequenceIndex] = *currentRank;
    } else {
      // We are looping, return NAN as computed result
      *values = static_cast<T>(NAN);
      return;
    }
    *values = s->approximateAtContextRank<T>(m_sequenceContext,
                                             intermediateComputation);
    m_smallestRankBeingComputed[sequenceIndex] = previousSmallestRank;
  }
}

template <typename T>
const Expression
TemplatedSequenceContext<T>::protectedExpressionForSymbolAbstract(
    const SymbolAbstract &symbol, bool clone,
    ContextWithParent *lastDescendantContext) {
  if (symbol.type() != ExpressionNode::Type::Sequence) {
    return ContextWithParent::protectedExpressionForSymbolAbstract(
        symbol, clone, lastDescendantContext);
  }
  T result = NAN;
  /* Do not use recordAtIndex : if the sequences have been reordered, the
   * name index and the record index may not correspond. */
  char name = static_cast<const Symbol &>(symbol).name()[0];
  int index = SequenceStore::SequenceIndexForName(name);
  Ion::Storage::Record record =
      m_sequenceContext->sequenceStore()->recordAtNameIndex(index);
  if (record.isNull()) {
    return Float<T>::Builder(result);
  }
  assert(record.fullName()[0] == symbol.name()[0]);
  Sequence *seq = m_sequenceContext->sequenceStore()->modelForRecord(record);
  if (!seq->fullName()) {
    return Float<T>::Builder(result);
  }
  Expression rankExpression = symbol.childAtIndex(0).clone();
  /* The lastDesendantContext might contain informations on variables
   * that are contained in the rank expression. */
  T rankValue = PoincareHelpers::ApproximateToScalar<T>(
      rankExpression, lastDescendantContext ? lastDescendantContext : this);
  result = m_sequenceContext->storedValueOfSequenceAtRank<T>(index, rankValue);
  /* We try to approximate the sequence independently from the others at the
   * required rank (this will solve u(n) = 5*n, v(n) = u(n+10) for instance).
   * But we avoid doing so if the sequence referencing itself to avoid an
   * infinite loop. */
  if (OMG::IsSignalingNan(result)) {
    // If the rank is not an int, return NAN
    if (std::floor(rankValue) == rankValue) {
      result = seq->approximateAtRank<T>(rankValue, m_sequenceContext);
    }
  }
  return Float<T>::Builder(result);
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

template <typename T>
TemplatedSequenceContext<T> *SequenceContext::context() {
  void *helper = sizeof(T) == sizeof(float) ? (void *)&m_floatSequenceContext
                                            : (void *)&m_doubleSequenceContext;
  return static_cast<TemplatedSequenceContext<T> *>(helper);
}

template class TemplatedSequenceContext<float>;
template class TemplatedSequenceContext<double>;
template TemplatedSequenceContext<float> *SequenceContext::context<float>();
template TemplatedSequenceContext<double> *SequenceContext::context<double>();

}  // namespace Shared
