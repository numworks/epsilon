#ifndef POINCARE_EXPRESSION_BOUNDS_H
#define POINCARE_EXPRESSION_BOUNDS_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

#include <algorithm>
#include <cmath>

namespace Poincare::Internal {

class Bounds {
 public:
  /* Get the sign of a Tree via double bounds, originally created to improve
   * the sign of difference e.g. pi - 1/2 */
  static Poincare::Sign Sign(const Tree* e);

  // Compute upper and lower bounds of a Tree
  static Bounds Compute(const Tree* e);

  bool areUndefined() const {
    return std::isnan(m_lower) || std::isnan(m_upper);
  }

  double lower() const { return m_lower; }
  double upper() const { return m_upper; }

 private:
  Bounds(double lower, double upper, uint8_t ulp = 1)
      : m_lower(lower), m_upper(upper) {
    spread(ulp);
  };
  constexpr Bounds() = default;
  static constexpr Bounds Invalid() { return Bounds(); }
  static Bounds Mult(const Tree* e);
  static Bounds Add(const Tree* e);
  static Bounds Pow(const Tree* e);
  void remove();
  void flip() { std::swap(m_lower, m_upper); }
  bool isStrictlyPositive() const {
    assert(exists());
    return 0 < m_lower;
  }
  bool isStrictlyNegative() const {
    assert(exists());
    return m_upper < 0;
  }
  bool isNull() const {
    assert(exists());
    return m_lower == 0 && m_upper == 0;
  }
  void applyMonotoneFunction(double (*f)(double), bool decreasing = false,
                             uint8_t ulp_precision = 1);
  /* Spread lower and upper bounds by specified ulp */
  void spread(unsigned int ulp_precision = 1);
  /* Check bounds are well defined */
  bool exists() const {
    // Both bounds are valid or neither
    assert(std::isfinite(m_lower) == std::isfinite(m_upper));
    return std::isfinite(m_lower);
  }
  /* Check if bounds exists and is valid, meaning either:
   * [lower <= upper < 0],
   * [0 < lower <= upper],
   * [lower == upper == 0]. */
  bool hasKnownStrictSign() const {
    return exists() && ((m_lower <= m_upper && (m_upper < 0 || 0 < m_lower)) ||
                        (m_lower == 0 && m_upper == 0));
  }

  double m_lower = NAN;
  double m_upper = NAN;
};

}  // namespace Poincare::Internal
#endif
