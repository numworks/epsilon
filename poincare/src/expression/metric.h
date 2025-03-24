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
#if USE_TREE_SIZE_METRIC
  // Metric of given tree. The smaller is the better.
  static int GetMetric(const Tree* e) {
    return (e->isDep() ? e->child(0) : e)->treeSize();
  }

#else
  /* Return a metric evaluating [e] complexity: the smaller the metric, the
   * simpler the tree. */
  static int GetTrueMetric(const Tree* e);
  /* Usually return [GetTrueMetric] result.
   * If a metric of [k_perfectMetric] is returned, it means that any equivalent
   * tree to [e] will have a worst metric, this is decided by
   * [CannotBeReducedFurther] */
  static int GetMetric(const Tree* e);
  /* Return [true] when any equivalent tree to [e] can be considered to have a
   * worst metric than [e]. */
  static bool CannotBeReducedFurther(const Tree* e);
  /* This metric is unreachable by any tree, but if a tree is considered to have
   * this metric then [CannotBeReducedFurther] is [true] */
  constexpr static int k_perfectMetric = 0;

 private:
  constexpr static int k_defaultMetric = 2 * 3 * 5;

  static int GetMetric(Type type);
#endif
};

}  // namespace Poincare::Internal

#endif
