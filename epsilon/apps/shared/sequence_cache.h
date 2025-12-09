#pragma once

#include <omg/troolean.h>

namespace Shared {

class SequenceStore;
class Sequence;

/* Store sequence values at rank to greatly reduce the number of computations.
 * Two caches are kept : a main one, and an intermediate one that is more
 * volatile and altered while computing the main one. For each sequences, those
 * caches are made of a rank and [k_storageDepth] stored values. The rank is the
 * highest rank of the stored values.
 * Other informations are also memoized here :
 * - The initial values of sequences ([k_storageDepth] starting from initial
 *   rank)
 * - If a sequence can be computed or not.
 */
class SequenceCache {
 public:
  SequenceCache(Shared::SequenceStore* sequenceStore);

  void resetCache();
  bool sequenceIsNotComputable(int sequenceIndex);
  // Update main cache values of [sequenceIndex] up to [rank].
  void stepUntilRank(int sequenceIndex, int rank);
  // Get value of sequence at [rank] if stored, or SignalingNan otherwise.
  double storedValueOfSequenceAtRank(int sequenceIndex, int rank);

 private:
  constexpr static int k_maxRecurrentRank = 10000;
  constexpr static int k_storageDepth = 6;
  constexpr static int k_numberOfSequences = 3;
  // SequenceStore::k_maxNumberOfSequences;

  void resetComputationStatus();
  const Shared::Sequence* sequenceAtNameIndex(int sequenceIndex) const;
  int rankForInitialValuesStorage(int sequenceIndex) const;

  // Following methods can be either called on main or intermediate cache.
  int* rankPointer(int sequenceIndex, bool intermediateComputation);
  int rank(int sequenceIndex, bool intermediateComputation) {
    return *(rankPointer(sequenceIndex, intermediateComputation));
  }
  double* valuesPointer(int sequenceIndex, bool intermediateComputation);
  // Shift rank and stored values by [delta] to the right (to higher ranks)
  void shiftCacheRight(int sequenceIndex, bool intermediateComputation,
                       int delta);
  void stepRanks(int sequenceIndex, bool intermediateComputation, int step);
  void resetValuesOfSequence(int sequenceIndex, bool intermediateComputation);
  void resetRanksAndValuesOfSequence(int sequenceIndex,
                                     bool intermediateComputation);

  /* Save initial values to avoid stepping all values back. For example if in an
   * intermediate computation we ask for v(n) with v(n+1) = v(n)+v(0), we would
   * otherwise always step to rank n and then step back to rank 0, replacing all
   * values stored in m_intermediateValues. */
  double m_initialValues[k_numberOfSequences][k_storageDepth];

  // If ranks are {9,5,4} then values are {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}.
  double m_mainValues[k_numberOfSequences][k_storageDepth];
  double m_intermediateValues[k_numberOfSequences][k_storageDepth];
  int m_mainRanks[k_numberOfSequences];
  int m_intermediateRanks[k_numberOfSequences];

  // Used to detect and escape infinite loops in stepRanks.
  int m_smallestRankBeingComputed[k_numberOfSequences];
  OMG::Troolean m_sequenceIsNotComputable[k_numberOfSequences];

  const Shared::SequenceStore* m_sequenceStore;
  /* In stepUntilRank, [m_isInsideComputation] is set to true so that following
   * nested calls to stepUntilRank will be applied on intermediate cache. */
  bool m_isInsideComputation;
};

}  // namespace Shared
