#ifndef POINCARE_DOMAIN_H
#define POINCARE_DOMAIN_H

#include <stdint.h>
#include <poincare/expression.h>
#include <poincare/float.h>
#include <poincare/rational.h>
#include <algorithm>

namespace Poincare {

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
    UnitSegment = 1 << 7,
    LeftOpenUnitSegment = 1 << 8,
    OpenUnitSegment = 1 << 9,
  };

  static constexpr Type nonZero = static_cast<Type>(NStar | RStar | RPlusStar | LeftOpenUnitSegment | OpenUnitSegment);
  static constexpr Type finite = static_cast<Type>(UnitSegment | LeftOpenUnitSegment | OpenUnitSegment);
  static constexpr Type onlyIntegers = static_cast<Type>(N | NStar);
  static constexpr Type onlyNegative = static_cast<Type>(RMinus);
  static constexpr Type onlyPositive = static_cast<Type>(N | NStar | RPlus | RPlusStar | UnitSegment | LeftOpenUnitSegment | OpenUnitSegment);

  template<typename T> static bool contains(Type type, T value) {
    if (std::isnan(value)) {
      return false;
    }
    if (std::isinf(value) && type & finite) {
      return false;
    }
    // TODO: should we test for integers; is inf an integer ?
    if (value == static_cast<T>(0.0) && type & nonZero) { // Epsilon ?
      return false;
    }
    if (value > static_cast<T>(0.0) && type & onlyNegative) {
      return false;
    }
    if (value < static_cast<T>(0.0) && type & onlyPositive) {
      return false;
    }
    if (value > static_cast<T>(1.0) && type & (UnitSegment | LeftOpenUnitSegment | OpenUnitSegment)) {
      return false;
    }
    if (value == static_cast<T>(1.0) && type & (OpenUnitSegment)) {
      return false;
    }
    return true;
  }

  /* Order is important :
   * - when casted to bool return if we should return early, ie non true
   * - the order is used when propagating results */
  enum Result {
    True = 0,
    CantCheck,
    False,
  };

  static Result expressionIsIn(const Expression &expression, Type domain, Context * context);

  static bool expressionIsIn(bool * result, const Expression &expression, Type domain, Context * context) {
    assert(result != nullptr);
    switch (expressionIsIn(expression, domain, context)) {
    case CantCheck:
      return false;
    case True:
      *result = true;
      return true;
    case False:
      *result = false;
      return true;
    }
  }

  static bool expressionsAreIn(bool * result, const Expression &expression1, Type domain1, const Expression &expression2, Type domain2, Context * context) {
    assert(result != nullptr);
    switch (std::max(expressionIsIn(expression1, domain1, context), expressionIsIn(expression2, domain2, context))) {
    case CantCheck:
      return false;
    case True:
      *result = true;
      return true;
    case False:
      *result = false;
      return true;
    }
  }
};

}

#endif
