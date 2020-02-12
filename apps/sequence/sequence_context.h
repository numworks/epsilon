#ifndef SEQUENCE_SEQUENCE_CONTEXT_H
#define SEQUENCE_SEQUENCE_CONTEXT_H

#include <poincare/context_with_parent.h>
#include <poincare/expression.h>
#include <poincare/symbol.h>

namespace Sequence {

constexpr static int MaxRecurrenceDepth = 2;
static constexpr int MaxNumberOfSequences = 3;

class SequenceStore;
class SequenceContext;

template<typename T>
class TemplatedSequenceContext {
public:
  TemplatedSequenceContext();
  T valueOfSequenceAtPreviousRank(int sequenceIndex, int rank) const;
  void resetCache();
  bool iterateUntilRank(int n, SequenceStore * sequenceStore, SequenceContext * sqctx);
private:
  constexpr static int k_maxRecurrentRank = 10000;
  /* Cache:
   * In order to accelerate the computation of values of recurrent sequences,
   * we memoize the last computed values of the sequence and their associated
   * ranks (n and n+1 for instance). Thereby, when another evaluation at a
   * superior rank k > n+1 is called, we avoid iterating from 0 but can start
   * from n. */
  void step(SequenceStore * sequenceStore, SequenceContext * sqctx);
  int m_rank;
  T m_values[MaxNumberOfSequences][MaxRecurrenceDepth+1];
};

class SequenceContext : public Poincare::ContextWithParent {
public:
  SequenceContext(Poincare::Context * parentContext, SequenceStore * sequenceStore) :
    ContextWithParent(parentContext),
    m_floatSequenceContext(),
    m_doubleSequenceContext(),
    m_sequenceStore(sequenceStore) {}
  /* expressionForSymbolAbstract & setExpressionForSymbolAbstractName directly call the parent
   * context respective methods. Indeed, special chars like n, u(n), u(n+1),
   * v(n), v(n+1) are taken into accound only when evaluating sequences which
   * is done in another context. */
  template<typename T> T valueOfSequenceAtPreviousRank(int sequenceIndex, int rank) const {
    if (sizeof(T) == sizeof(float)) {
      return m_floatSequenceContext.valueOfSequenceAtPreviousRank(sequenceIndex, rank);
    }
    return m_doubleSequenceContext.valueOfSequenceAtPreviousRank(sequenceIndex, rank);
  }
  void resetCache() {
    m_floatSequenceContext.resetCache();
    m_doubleSequenceContext.resetCache();
  }
  template<typename T> bool iterateUntilRank(int n) {
    if (sizeof(T) == sizeof(float)) {
      return m_floatSequenceContext.iterateUntilRank(n, m_sequenceStore, this);
    }
    return m_doubleSequenceContext.iterateUntilRank(n, m_sequenceStore, this);
  }
private:
  TemplatedSequenceContext<float> m_floatSequenceContext;
  TemplatedSequenceContext<double> m_doubleSequenceContext;
  SequenceStore * m_sequenceStore;
};

}

#endif
