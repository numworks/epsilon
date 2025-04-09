#ifndef POINCARE_EXPRESSION_BOUNDS_H
#define POINCARE_EXPRESSION_BOUNDS_H

#include <poincare/sign.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Bounds {
 public:
  /* Get the sign of a Tree via double bounds, originally created to improve
   * the sign of difference e.g. pi - 1/2 */
  static Poincare::Sign Sign(const Tree* e);

 private:
  double m_lower;
  double m_upper;
  Bounds(double lower, double upper, uint8_t ulp = 1)
      : m_lower(lower), m_upper(upper) {
    spread(ulp);
  };
  Bounds() : m_lower(NAN), m_upper(NAN){};
  static Bounds Invalid() { return Bounds(); }
  static Bounds Compute(const Tree* e);
  static Bounds Mult(const Tree* e);
  static Bounds Add(const Tree* e);
  static Bounds Pow(const Tree* e);
  void remove();
  void flip();
  bool isStrictlyPositive() {
    assert(exists());
    return 0 < m_lower;
  }
  bool isStrictlyNegative() {
    assert(exists());
    return m_upper < 0;
  }
  bool isNull() {
    assert(exists());
    return m_lower == 0 && m_upper == 0;
  }
  void applyMonotoneFunction(double (*f)(double), bool decreasing = false,
                             uint8_t ulp_precision = 1);
  /* Spread lower and upper bounds by specified ulp */
  void spread(unsigned int ulp_precision = 1);
  /* Check bounds are well defined */
  bool exists() {
    // Both bounds are valid or neither
    assert(std::isfinite(m_lower) == std::isfinite(m_upper));
    return std::isfinite(m_lower);
  }
  /* Check if bounds exists and is valid, meaning either:
   * [lower <= upper < 0],
   * [0 < lower <= upper],
   * [lower == upper == 0]. */
  bool hasKnownStrictSign() {
    return exists() && ((m_lower <= m_upper && (m_upper < 0 || 0 < m_lower)) ||
                        (m_lower == 0 && m_upper == 0));
  }
};

}  // namespace Poincare::Internal
#endif
