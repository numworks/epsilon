#ifndef SOLVER_SOLUTION_H
#define SOLVER_SOLUTION_H

#include <poincare/layout.h>

namespace Solver {

class Solution {
 public:
  enum class ApproximationType : uint8_t {
    Identical,
    Equal,
    Approximate,
  };

  Solution()
      : m_approximate(NAN),
        m_exactLayout(),
        m_type(ApproximationType::Identical) {}
  Solution(Poincare::Layout exactLayout, Poincare::Layout approximateLayout,
           double approximate, ApproximationType type)
      : m_approximate(approximate),
        m_exactLayout(exactLayout),
        m_approximateLayout(approximateLayout),
        m_type(type) {}

  Poincare::Layout exactLayout() const { return m_exactLayout; }
  double approximate() const { return m_approximate; }
  Poincare::Layout approximateLayout() const { return m_approximateLayout; }
  ApproximationType approximationType() const { return m_type; }

 private:
  double m_approximate;
  Poincare::Layout m_exactLayout;
  Poincare::Layout m_approximateLayout;
  ApproximationType m_type;
};

}  // namespace Solver

#endif
