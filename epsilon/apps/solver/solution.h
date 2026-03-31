#pragma once

#include <poincare/layout.h>

namespace Solver {

class Solution {
 public:
  Solution() : m_approximate(NAN), m_comparisonSignIsEqual(true) {}
  Solution(const Poincare::Layout& exactLayout,
           const Poincare::Layout& approximateLayout, double approximate,
           bool comparisonSignIsEqual)
      : m_approximate(approximate),
        m_exactLayout(exactLayout),
        m_approximateLayout(approximateLayout),
        m_comparisonSignIsEqual(comparisonSignIsEqual) {}

  Poincare::Layout exactLayout() const { return m_exactLayout; }
  double approximate() const { return m_approximate; }
  Poincare::Layout approximateLayout() const { return m_approximateLayout; }
  bool comparisonSignIsEqual() const { return m_comparisonSignIsEqual; }
  bool comparisonSignIsVisible() const {
    // TODO: Add m_comparisonSignIsVisible
    return true;
  }

 private:
  double m_approximate;
  Poincare::Layout m_exactLayout;
  Poincare::Layout m_approximateLayout;
  bool m_comparisonSignIsEqual;
};

}  // namespace Solver
