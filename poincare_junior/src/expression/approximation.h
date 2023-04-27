#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <poincare_junior/src/memory/node.h>

#include <cmath>

namespace PoincareJ {

/* Approximation is implemented on all block types.
 * We could have asserted that we reduce before approximating (and thus
 * implemented the approximation only on internal types) but this increases the
 * number of operations (for instance, 2 / 3 VS 2 * 3 ^ -1) and decreases the
 * precision. We rather beautify before approximating.
 */

class Approximation final {
 public:
  template <typename T>
  static T To(const Node node);

  template <typename T>
  static T FloatAddition(T a, T b) {
    return a + b;
  }
  template <typename T>
  static T FloatMultiplication(T a, T b) {
    return a * b;
  }
  template <typename T>
  static T FloatPower(T a, T b) {
    return std::pow(a, b);
  }
  template <typename T>
  static T FloatSubtraction(T a, T b) {
    return a - b;
  }
  template <typename T>
  static T FloatDivision(T a, T b) {
    return a / b;
  }
  template <typename T>
  static T FloatLog(T a, T b) {
    return std::log2(a) / std::log2(b);
  }
  template <typename T>
  static T FloatTrig(T a, T b) {
    assert(b == -1.0 || b == 0.0 || b == 1.0 || b == 2.0);
    return ((b == -1.0 || b == 2.0) ? -1.0 : 1.0) *
           (((static_cast<int>(b) + 2) % 2 == 0) ? std::cos(a) : std::sin(a));
  }

 private:
  template <typename T>
  using Reductor = T (*)(T, T);
  template <typename T>
  static T MapAndReduce(const Node node, Reductor<T> reductor);
};

}  // namespace PoincareJ

#endif
