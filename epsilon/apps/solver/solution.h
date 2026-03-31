#pragma once

#include <poincare/layout.h>

namespace Solver {

class Solution {
 public:
  Solution() : m_approximate(NAN), m_comparisonSignIsEqual(true) {}
  Solution(const Poincare::Layout& exactLayout,
           const Poincare::Layout& approximateLayout, double approximate,
           bool comparisonSignIsEqual, bool comparisonSignIsVisible)
      : m_approximate(approximate),
        m_exactLayout(exactLayout),
        m_approximateLayout(approximateLayout),
        m_comparisonSignIsEqual(comparisonSignIsEqual),
        m_comparisonSignIsVisible(comparisonSignIsVisible) {}

  Poincare::Layout exactLayout() const { return m_exactLayout; }
  double approximate() const { return m_approximate; }
  Poincare::Layout approximateLayout() const { return m_approximateLayout; }
  bool comparisonSignIsEqual() const { return m_comparisonSignIsEqual; }
  bool comparisonSignIsVisible() const { return m_comparisonSignIsVisible; }

 private:
  double m_approximate;
  Poincare::Layout m_exactLayout;
  Poincare::Layout m_approximateLayout;
  bool m_comparisonSignIsEqual;
  bool m_comparisonSignIsVisible;
};

}  // namespace Solver
