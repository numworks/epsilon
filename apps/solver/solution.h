#ifndef SOLVER_SOLUTION_H
#define SOLVER_SOLUTION_H

#include <poincare/layout.h>

namespace Solver {

class Solution {
 public:
  Solution() : m_approximate(NAN) {}
  Solution(Poincare::Layout exactLayout, Poincare::Layout approximateLayout,
           double approximate, bool exactAndApproximateAreEqual)
      : m_approximate(approximate),
        m_exactLayout(exactLayout),
        m_approximateLayout(approximateLayout),
        m_exactAndApproximateAreEqual(exactAndApproximateAreEqual) {}

  Poincare::Layout exactLayout() const { return m_exactLayout; }
  double approximate() const { return m_approximate; }
  Poincare::Layout approximateLayout() const { return m_approximateLayout; }
  bool exactAndApproximateAreEqual() const {
    return m_exactAndApproximateAreEqual;
  }

 private:
  double m_approximate;
  Poincare::Layout m_exactLayout;
  Poincare::Layout m_approximateLayout;
  bool m_exactAndApproximateAreEqual;
};

}  // namespace Solver

#endif
