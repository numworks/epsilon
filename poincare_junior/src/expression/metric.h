#ifndef POINCARE_EXPRESSION_METRIC_H
#define POINCARE_EXPRESSION_METRIC_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* TODO_PCJ: Metric should never return a same score for two different
 * expressions. */
class Metric {
 public:
  // Metric of given tree. The smaller is the better.
  static int GetMetric(const Tree* u) { return u->treeSize(); }

#if 0
  static int GetMetric(const Tree* u);

 private:
  constexpr static int k_defaultMetric = 2 * 3 * 5;

  static int GetMetric(Type type);
#endif
};

}  // namespace PoincareJ

#endif
