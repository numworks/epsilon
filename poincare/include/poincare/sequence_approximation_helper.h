#pragma once

namespace Poincare {

/* SequenceApproximationHelper is necessary because implicit sequences are not
 * replaced by their definition at projection (by exception, all other symbols
 * should have disappeared after projection). For implicit sequences, it is
 * needed to request the sequence definition.
 * SequenceApproximationHelper holds a pointer to a static Context instance
 * (that generally lives in the Apps of the project). This Context can fetch the
 * implicit sequence definition from its indentifier, and is capable of
 * approximating the value of the sequence at a certain rank.
 */

class SequenceApproximationHelper {
 public:
  class ContextInterface {
   public:
    virtual double approximateSequenceAtRank(const char* identifier,
                                             int rank) const = 0;
  };

  static double ApproximateSequenceAtRank(const char* identifier, int rank) {
    return s_sequenceContext->approximateSequenceAtRank(identifier, rank);
  }

  static inline void Init(
      const SequenceApproximationHelper::ContextInterface* sequenceContext) {
    s_sequenceContext = sequenceContext;
  }

 private:
  static inline const SequenceApproximationHelper::ContextInterface*
      s_sequenceContext = nullptr;
};

}  // namespace Poincare
