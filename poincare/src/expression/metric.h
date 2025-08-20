#ifndef POINCARE_EXPRESSION_METRIC_H
#define POINCARE_EXPRESSION_METRIC_H

#include <poincare/src/memory/tree.h>

#include "context.h"

#define USE_TREE_SIZE_METRIC 0

namespace Poincare::Internal {

/* TODO: Metric should never return a same score for two different
 * expressions. AdvanceReduction uses this flawed metric but handles identical
 * metrics on different expressions by looking at their CRC. */
class Metric {
 public:
#if USE_TREE_SIZE_METRIC
  // Metric of given tree. The smaller is the better.
  static int GetMetric(const Tree* e, ReductionTarget reductionTarget) {
    return (e->isDep() ? e->child(0) : e)->treeSize();
  }

#else
  /* Return a metric evaluating [e] complexity: the smaller the metric, the
   * simpler the tree. */
  static float GetTrueMetric(const Tree* e, ReductionTarget reductionTarget);
  /* Usually return [GetTrueMetric] result.
   * If a metric of [k_perfectMetric] is returned, it means that any equivalent
   * tree to [e] will have a worst metric, this is decided by
   * [CannotBeReducedFurther] */
  static float GetMetric(const Tree* e, ReductionTarget reductionTarget);
  /* Return [true] when any equivalent tree to [e] can be considered to have a
   * worst metric than [e]. */
  static bool CannotBeReducedFurther(const Tree* e);
  /* This metric is unreachable by any tree, but if a tree is considered to have
   * this metric then [CannotBeReducedFurther] is [true] */
  constexpr static float k_perfectMetric = 0.f;

 private:
  constexpr static float k_defaultMetric = 1.f;

  static float GetMetric(Type type);
  static float GetAddMultMetric(const Tree* e);
#endif
};

}  // namespace Poincare::Internal

#endif
