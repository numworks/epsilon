#include "sequence_cache.h"

#include <assert.h>
#include <omg/signaling_nan.h>

// TODO_PCJ remove this include
#include <apps/shared/sequence_store.h>

namespace Poincare::Internal {

SequenceCache::SequenceCache(Shared::SequenceStore* sequenceStore)
    : m_sequenceStore(sequenceStore) {
  resetCache();
}

void SequenceCache::resetValuesOfSequence(int sequenceIndex,
                                          bool intermediateComputation) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  double* values = valuesPointer(sequenceIndex, intermediateComputation);
  for (int depth = 0; depth < k_storageDepth; depth++) {
    *(values + depth) = OMG::SignalingNan<double>();
  }
}

void SequenceCache::resetRanksAndValuesOfSequence(
    int sequenceIndex, bool intermediateComputation) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  int* rank = rankPointer(sequenceIndex, intermediateComputation);
  *rank = -1;
  resetValuesOfSequence(sequenceIndex, intermediateComputation);
}

void SequenceCache::resetComputationStatus() {
  m_isInsideComputation = false;
  for (int i = 0; i < k_numberOfSequences; i++) {
    m_smallestRankBeingComputed[i] = -1;
  }
}

double SequenceCache::storedValueOfSequenceAtRank(int sequenceIndex, int rank) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  for (int loop = 0; loop < 3; loop++) {
    int storedRank = loop < 2 ? *(rankPointer(sequenceIndex, loop))
                              : rankForInitialValuesStorage(sequenceIndex);
    if (storedRank >= 0) {
      int offset = storedRank - rank;
      if (0 <= offset && offset < k_storageDepth) {
        double storedValue =
            loop < 2 ? *(valuesPointer(sequenceIndex, loop) + offset)
                     : m_initialValues[sequenceIndex][offset];
        if (!OMG::IsSignalingNan(storedValue)) {
          return storedValue;
        }
      }
    }
  }
  return OMG::SignalingNan<double>();
}

int* SequenceCache::rankPointer(int sequenceIndex,
                                bool intermediateComputation) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  int* rank = intermediateComputation ? m_intermediateRanks : m_mainRanks;
  rank += sequenceIndex;
  return rank;
}

double* SequenceCache::valuesPointer(int sequenceIndex,
                                     bool intermediateComputation) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  double* values = intermediateComputation
                       ? reinterpret_cast<double*>(&m_intermediateValues)
                       : reinterpret_cast<double*>(&m_mainValues);
  values += sequenceIndex * (k_storageDepth);
  return values;
}

void SequenceCache::shiftValuesRight(int sequenceIndex,
                                     bool intermediateComputation, int delta) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  assert(delta > 0);
  if (delta >= k_storageDepth) {
    resetValuesOfSequence(sequenceIndex, intermediateComputation);
    return;
  }
  double* values = valuesPointer(sequenceIndex, intermediateComputation);
  int stop = delta - 1;
  assert(0 <= stop && stop < k_storageDepth - 1);
  for (int depth = k_storageDepth - 1; depth > stop; depth--) {
    *(values + depth) = *(values + depth - delta);
  }
  for (int depth = stop; depth >= 0; depth--) {
    *(values + depth) = OMG::SignalingNan<double>();
  }
}

void SequenceCache::stepUntilRank(int sequenceIndex, int rank) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  bool intermediateComputation = m_isInsideComputation;
  const Shared::Sequence* s = sequenceAtNameIndex(sequenceIndex);
  assert(s->isDefined());
  assert(rank >= s->initialRank());
  bool explicitComputation = rank >= s->firstNonInitialRank() &&
                             s->canBeHandledAsExplicit(nullptr /* TODO this*/);
  if (!explicitComputation && rank > k_maxRecurrentRank) {
    return;
  }

  double cacheValue = storedValueOfSequenceAtRank(sequenceIndex, rank);
  bool jumpToRank = explicitComputation || !OMG::IsSignalingNan(cacheValue);

  int* currentRank = rankPointer(sequenceIndex, intermediateComputation);
  if (*currentRank > rank) {
    resetRanksAndValuesOfSequence(sequenceIndex, intermediateComputation);
  }
  while (*currentRank < rank) {
    int step = jumpToRank ? rank - *currentRank : 1;
    stepRanks(sequenceIndex, intermediateComputation, step);
  }
}

void SequenceCache::stepRanks(int sequenceIndex, bool intermediateComputation,
                              int step) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);

  // Update computation state
  m_isInsideComputation = true;

  int* currentRank = rankPointer(sequenceIndex, intermediateComputation);
  double* values = valuesPointer(sequenceIndex, intermediateComputation);

  // First we increment the rank
  *currentRank += step;

  // Then we shift the values
  shiftValuesRight(sequenceIndex, intermediateComputation, step);

  // We approximate the value at new rank
  double otherCacheValue =
      storedValueOfSequenceAtRank(sequenceIndex, *currentRank);
  if (!OMG::IsSignalingNan(otherCacheValue)) {
    // If the other cache already knows this value, use it
    *values = otherCacheValue;
  } else {
    int previousSmallestRank = m_smallestRankBeingComputed[sequenceIndex];
    if (previousSmallestRank == -1 || *currentRank < previousSmallestRank) {
      m_smallestRankBeingComputed[sequenceIndex] = *currentRank;
    } else {
      // We are looping, return NAN as computed result
      *values = static_cast<double>(NAN);
      return;
    }
    const Shared::Sequence* s = sequenceAtNameIndex(sequenceIndex);
    assert(s->isDefined());
    *values = s->approximateAtContextRank(
        nullptr, rank(sequenceIndex, intermediateComputation),
        intermediateComputation);
    m_smallestRankBeingComputed[sequenceIndex] = previousSmallestRank;
    // Store value in initial storage if rank is in the right range
    int offset = rankForInitialValuesStorage(sequenceIndex) - *currentRank;
    if (0 <= offset && offset < k_storageDepth) {
      m_initialValues[sequenceIndex][offset] = *values;
    }
  }

  // Update computation state
  if (!intermediateComputation) {
    resetComputationStatus();
  }
}

void SequenceCache::resetCache() {
  for (int i = 0; i < k_numberOfSequences; i++) {
    resetRanksAndValuesOfSequence(i, true);
    resetRanksAndValuesOfSequence(i, false);
    for (int j = 0; j < k_storageDepth; ++j) {
      m_initialValues[i][j] = OMG::SignalingNan<double>();
    }
  }
  resetComputationStatus();
  for (int i = 0; i < k_numberOfSequences; i++) {
    m_sequenceIsNotComputable[i] = OMG::Troolean::Unknown;
  }
}

const Shared::Sequence* SequenceCache::sequenceAtNameIndex(
    int sequenceIndex) const {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  Ion::Storage::Record record =
      m_sequenceStore->recordAtNameIndex(sequenceIndex);
  assert(!record.isNull());
  const Shared::Sequence* s = m_sequenceStore->modelForRecord(record);
  return s;
}

int SequenceCache::rankForInitialValuesStorage(int sequenceIndex) const {
  return sequenceAtNameIndex(sequenceIndex)->initialRank() + k_storageDepth - 1;
}

bool SequenceCache::sequenceIsNotComputable(int sequenceIndex) {
  assert(0 <= sequenceIndex && sequenceIndex < k_numberOfSequences);
  if (m_sequenceIsNotComputable[sequenceIndex] == OMG::Troolean::Unknown) {
    m_sequenceIsNotComputable[sequenceIndex] =
        OMG::BoolToTroolean(sequenceAtNameIndex(sequenceIndex)
                                ->mainExpressionIsNotComputable(nullptr));
  }
  return TrooleanToBool(m_sequenceIsNotComputable[sequenceIndex]);
}

}  // namespace Poincare::Internal
