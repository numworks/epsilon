#pragma once

#include <poincare/layout.h>

namespace Solver {

class Solution {
 public:
  Solution() : m_approximate(NAN), m_exactAndApproximateAreEqual(true) {}
  Solution(const Poincare::Layout& exactLayout,
           const Poincare::Layout& approximateLayout, double approximate,
           bool exactAndApproximateAreEqual)
      : m_approximate(approximate),
        m_exactLayout(exactLayout),
        m_approximateLayout(approximateLayout),
        m_exactAndApproximateAreEqual(exactAndApproximateAreEqual) {}

  Poincare::Layout exactLayout() const { return m_exactLayout; }
  double approximate() const { return m_approximate; }
  Poincare::Layout approximateLayout() const { return m_approximateLayout; }
  bool comparisonSignIsEqual() const { return m_exactAndApproximateAreEqual; }
  bool comparisonSignIsVisible() const {
    // TODO: Handle this
    return true;
  }

 private:
  double m_approximate;
  Poincare::Layout m_exactLayout;
  Poincare::Layout m_approximateLayout;
  bool m_exactAndApproximateAreEqual;
};

}  // namespace Solver
