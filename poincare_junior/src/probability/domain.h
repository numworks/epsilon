#ifndef POINCARE_JUNIOR_DOMAIN_H
#define POINCARE_JUNIOR_DOMAIN_H

#include <omgpj/troolean.h>
#include <poincare/float.h>
#include <poincare/rational.h>
#include <poincare_junior/src/memory/tree.h>
#include <stdint.h>

#include <algorithm>

namespace PoincareJ {

class Context;

class Domain {
 public:
  enum Type : uint16_t {
    N = 1 << 0,
    NStar = 1 << 1,
    R = 1 << 2,
    RStar = 1 << 3,
    RPlus = 1 << 4,
    RPlusStar = 1 << 5,
    RMinus = 1 << 6,
    UnitSegment = 1 << 7,          // [0, 1]
    LeftOpenUnitSegment = 1 << 8,  // ]0, 1]
    OpenUnitSegment = 1 << 9,      // ]0, 1[
  };

  constexpr static Type k_nonZero = static_cast<Type>(
      NStar | RStar | RPlusStar | LeftOpenUnitSegment | OpenUnitSegment);
  constexpr static Type k_finite =
      static_cast<Type>(UnitSegment | LeftOpenUnitSegment | OpenUnitSegment);
  constexpr static Type k_onlyIntegers = static_cast<Type>(N | NStar);
  constexpr static Type k_onlyNegative = static_cast<Type>(RMinus);
  constexpr static Type k_onlyPositive =
      static_cast<Type>(N | NStar | RPlus | RPlusStar | UnitSegment |
                        LeftOpenUnitSegment | OpenUnitSegment);

  template <typename T>
  static bool Contains(T value, Type type) {
    if (std::isnan(value)) {
      return false;
    }
    if (std::isinf(value) && type & k_finite) {
      return false;
    }
    // TODO: should we test for integers; is inf an integer ?
    if (value == static_cast<T>(0.0) && type & k_nonZero) {  // Epsilon ?
      return false;
    }
    if (value > static_cast<T>(0.0) && type & k_onlyNegative) {
      return false;
    }
    if (value < static_cast<T>(0.0) && type & k_onlyPositive) {
      return false;
    }
    if (value > static_cast<T>(1.0) &&
        type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment)) {
      return false;
    }
    if (value == static_cast<T>(1.0) && type & (OpenUnitSegment)) {
      return false;
    }
    return true;
  }

  static Troolean ExpressionIsIn(const Tree *expression, Type domain,
                                 Context *context);

  static bool ExpressionIsIn(bool *result, const Tree *expression, Type domain,
                             Context *context) {
    assert(result != nullptr);
    Troolean expressionsIsIn = ExpressionIsIn(expression, domain, context);
    switch (expressionsIsIn) {
      case Troolean::Unknown:
        return false;
      case Troolean::True:
        *result = true;
        return true;
      default:
        assert(expressionsIsIn == Troolean::False);
        *result = false;
        return true;
    }
  }

  static bool ExpressionsAreIn(bool *result, const Tree *expression1,
                               Type domain1, const Tree *expression2,
                               Type domain2, Context *context) {
    assert(result != nullptr);
    Troolean expressionsAreIn =
        TrinaryAnd(ExpressionIsIn(expression1, domain1, context),
                   ExpressionIsIn(expression2, domain2, context));
    switch (expressionsAreIn) {
      case Troolean::Unknown:
        return false;
      case Troolean::True:
        *result = true;
        return true;
      default:
        assert(expressionsAreIn == Troolean::False);
        *result = false;
        return true;
    }
  }
};

}  // namespace PoincareJ

#endif
