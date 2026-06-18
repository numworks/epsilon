#pragma once

#include <assert.h>

namespace Poincare::Internal::Approximation {

// Boolean return type used for Approximation
class BooleanOrUndefined {
 public:
  // Boolean of given value
  constexpr BooleanOrUndefined(bool value) : m_value{value} {}
  // Undefined boolean
  constexpr static BooleanOrUndefined Undef() { return BooleanOrUndefined(); }

  bool isUndefined() const { return m_isUndefined; }
  bool value() const {
    assert(!m_isUndefined);
    return m_value;
  }

 private:
  constexpr BooleanOrUndefined() : m_isUndefined{true} {}
  bool m_isUndefined = false;
  /* In case of an undefined, the stored value is false (but it should not be
   * fetched) */
  bool m_value = false;
};

}  // namespace Poincare::Internal::Approximation
