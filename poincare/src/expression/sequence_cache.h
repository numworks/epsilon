#ifndef POINCARE_EXPRESSION_SEQUENCE_CACHE_H
#define POINCARE_EXPRESSION_SEQUENCE_CACHE_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class SequenceContext : public Poincare::ContextWithParent {
 public:
  SequenceContext(Poincare::Context* parentContext,
                  SequenceStore* sequenceStore);

  void resetCache();
  bool sequenceIsNotComputable(int sequenceIndex);

  void stepUntilRank(int sequenceIndex, int rank);
  int rank(int sequenceIndex, bool intermediateComputation) {
    return *(rankPointer(sequenceIndex, intermediateComputation));
  }
  double storedValueOfSequenceAtRank(int sequenceIndex, int rank);

 private:
  constexpr static int k_maxRecurrentRank = 10000;
  constexpr static int k_storageDepth = 6;
  constexpr static int k_numberOfSequences =
      SequenceStore::k_maxNumberOfSequences;

  int* rankPointer(int sequenceIndex, bool intermediateComputation);
  double* valuesPointer(int sequenceIndex, bool intermediateComputation);
  void shiftValuesRight(int sequenceIndex, bool intermediateComputation,
                        int delta);
  void stepRanks(int sequenceIndex, bool intermediateComputation, int step);
  void resetValuesOfSequence(int sequenceIndex, bool intermediateComputation);
  void resetRanksAndValuesOfSequence(int sequenceIndex,
                                     bool intermediateComputation);
  void resetComputationStatus();
  Sequence* sequenceAtNameIndex(int sequenceIndex) const;
  int rankForInitialValuesStorage(int sequenceIndex) const;

  /* Main ranks for main computations and intermediate ranks for intermediate
   * computations (ex: computation of v(2) in u(3) = v(2) + 4). If ranks are
   * {9,5,4} then values are {{u9,u8,u7}, {v5,v4,v3}, {w4,w3,w2}}. */
  int m_mainRanks[k_numberOfSequences];
  double m_mainValues[k_numberOfSequences][k_storageDepth];
  int m_intermediateRanks[k_numberOfSequences];
  double m_intermediateValues[k_numberOfSequences][k_storageDepth];
  /* Save initial values to avoid stepping all values back. For example if in an
   * intermediate computation we ask for v(n) with v(n+1) = v(n)+v(0), we will
   * always step to rank n and then step back to rank 0, replacing all values
   * stored in m_intermediateValues. */
  double m_initialValues[k_numberOfSequences][k_storageDepth];

  bool m_isInsideComputation;
  int m_smallestRankBeingComputed[k_numberOfSequences];
};

}  // namespace Poincare::Internal
#endif
