#ifndef POINCARE_EXPRESSION_APPROXIMATION_H
#define POINCARE_EXPRESSION_APPROXIMATION_H

#include <cmath>
#include <poincare_junior/src/memory/node.h>

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
  static T FloatAddition(T a, T b) { return a + b; }
  template <typename T>
  static T FloatMultiplication(T a, T b) { return a * b; }
  template <typename T>
  static T FloatPower(T a, T b) { return std::pow(a, b); }
  template <typename T>
  static T FloatSubtraction(T a, T b) { return a - b; }
  template <typename T>
  static T FloatDivision(T a, T b) { return a / b; }
private:
  template <typename T>
  using Reductor = T (*)(T,T);
  template <typename T>
  static T MapAndReduce(const Node node, Reductor<T> reductor);
};

}

#endif
