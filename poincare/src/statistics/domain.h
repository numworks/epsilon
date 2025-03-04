#ifndef POINCARE_STATISTICS_PROBABILITY_DOMAIN_H
#define POINCARE_STATISTICS_PROBABILITY_DOMAIN_H

#include <omg/float.h>
#include <omg/troolean.h>
#include <poincare/src/memory/tree.h>
#include <stdint.h>

#include <algorithm>

namespace Poincare::Internal {

namespace Domain {
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
  ZeroToOneExcluded = 1 << 10,         // [0, 1[
};

constexpr static Type k_nonZero = static_cast<Type>(
    NStar | RStar | RPlusStar | ZeroExcludedToOne | ZeroExcludedToOneExcluded);
constexpr static Type k_finite =
    static_cast<Type>(ZeroToOne | ZeroExcludedToOne |
                      ZeroExcludedToOneExcluded | ZeroToOneExcluded);
constexpr static Type k_onlyIntegers = static_cast<Type>(N | NStar);
constexpr static Type k_onlyNegative = static_cast<Type>(RMinus);
constexpr static Type k_onlyPositive = static_cast<Type>(
    N | NStar | RPlus | RPlusStar | ZeroToOne | ZeroExcludedToOne |
    ZeroExcludedToOneExcluded | ZeroToOneExcluded);

template <typename T>
static bool ContainsFloat(T value, Type type) {
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
  if (value == static_cast<T>(1.0) &&
      type & (ZeroExcludedToOneExcluded | ZeroToOneExcluded)) {
    return false;
  }
  if (std::floor(value) != value && type & k_onlyIntegers) {
    return false;
  }
  return true;
}

template <typename T>
OMG::Troolean Contains(const T val, Type domain);

// Returns true if val1 >= val2
template <typename T>
OMG::Troolean IsAGreaterThanB(const T a, const T b, bool strictly = false);

};  // namespace Domain

}  // namespace Poincare::Internal

#endif
