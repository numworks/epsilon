#ifndef POINCARE_EXPRESSION_METRIC_H
#define POINCARE_EXPRESSION_METRIC_H

#include <poincare/src/memory/tree.h>

#define USE_TREE_SIZE_METRIC 0

namespace Poincare::Internal {

/* TODO: Metric should never return a same score for two different
 * expressions. AdvanceReduction uses this flawed metric but handles identical
 * metrics on different expressions by looking at their CRC. */
class Metric {
 public:
  // Metric of given tree. The smaller is the better.
#if USE_TREE_SIZE_METRIC
  static int GetMetric(const Tree* e) {
    return (e->isDep() ? e->child(0) : e)->treeSize();
  }

#else
  static int GetMetric(const Tree* e);

 private:
  constexpr static int k_defaultMetric = 2 * 3 * 5;

  static int GetMetric(Type type);
#endif
};

}  // namespace Poincare::Internal

#endif
