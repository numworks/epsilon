#ifndef POINCARE_PROBABILITY_DOMAIN_H
#define POINCARE_PROBABILITY_DOMAIN_H

#include <omg/float.h>
#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>
#include <stdint.h>

#include <algorithm>

namespace Poincare::Internal {

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
    ZeroToOne = 1 << 7,                  // [0, 1]
    ZeroExcludedToOne = 1 << 8,          // ]0, 1]
    ZeroExcludedToOneExcluded = 1 << 9,  // ]0, 1[
  };

  constexpr static Type k_nonZero =
      static_cast<Type>(NStar | RStar | RPlusStar | ZeroExcludedToOne |
                        ZeroExcludedToOneExcluded);
  constexpr static Type k_finite = static_cast<Type>(
      ZeroToOne | ZeroExcludedToOne | ZeroExcludedToOneExcluded);
  constexpr static Type k_onlyIntegers = static_cast<Type>(N | NStar);
  constexpr static Type k_onlyNegative = static_cast<Type>(RMinus);
  constexpr static Type k_onlyPositive =
      static_cast<Type>(N | NStar | RPlus | RPlusStar | ZeroToOne |
                        ZeroExcludedToOne | ZeroExcludedToOneExcluded);

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
        type & (ZeroToOne | ZeroExcludedToOne | ZeroExcludedToOneExcluded)) {
      return false;
    }
    if (value == static_cast<T>(1.0) && type & (ZeroExcludedToOneExcluded)) {
      return false;
    }
    return true;
  }

  static OMG::Troolean ExpressionIsIn(const Tree* e, Type domain);

  static bool ExpressionIsIn(bool* result, const Tree* e, Type domain) {
    assert(result != nullptr);
    OMG::Troolean isIn = ExpressionIsIn(e, domain);
    switch (isIn) {
      case OMG::Troolean::Unknown:
        return false;
      case OMG::Troolean::True:
        *result = true;
        return true;
      default:
        assert(isIn == OMG::Troolean::False);
        *result = false;
        return true;
    }
  }

  static bool ExpressionsAreIn(bool* result, const Tree* e1, Type domain1,
                               const Tree* e2, Type domain2) {
    assert(result != nullptr);
    OMG::Troolean areIn =
        TrooleanAnd(ExpressionIsIn(e1, domain1), ExpressionIsIn(e2, domain2));
    switch (areIn) {
      case OMG::Troolean::Unknown:
        return false;
      case OMG::Troolean::True:
        *result = true;
        return true;
      default:
        assert(areIn == OMG::Troolean::False);
        *result = false;
        return true;
    }
  }
};

}  // namespace Poincare::Internal

#endif
