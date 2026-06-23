#pragma once

#include <omg/bit_helper.h>
#include <poincare/src/expression/dimension_type.h>
#include <stdint.h>

namespace Poincare {

namespace AllowedDimensionsInternal {
/* This method cannot be a static method of the AllowedDimensions class for
 * technical reasons (the constexpr static members of AllowedDimensions need to
 * be initialized by a constant expression)  */
constexpr uint8_t TypeFlag(Internal::DimensionType type) {
  static_assert(sizeof(Internal::DimensionType) == 1);
  return 1 << static_cast<uint8_t>(type);
}
}  // namespace AllowedDimensionsInternal

/* Precondition on an expression's dimension. The class holds on one byte and
 * uses bit fields:
 *  - one bit per DimensionType for the allowed element types,
 *  - one bit for whether being a list is allowed.
 * The list bit is orthogonal to the element type. */
class AllowedDimensions {
 public:
  constexpr bool allows(Internal::DimensionType type) const {
    return OMG::BitHelper::bitAtIndex(m_bits, static_cast<size_t>(type));
  }
  constexpr bool listAllowed() const {
    return OMG::BitHelper::bitAtIndex(m_bits, k_listBit);
  }

  constexpr static AllowedDimensions Any() {
    return AllowedDimensions(k_allTypes | k_listFlag);
  }
  constexpr static AllowedDimensions AnyNoUnit() {
    using AllowedDimensionsInternal::TypeFlag;
    return AllowedDimensions(
        (k_allTypes & ~TypeFlag(Internal::DimensionType::Unit)) | k_listFlag);
  }
  // No list, no unit
  constexpr static AllowedDimensions PureScalar() {
    return AllowedDimensions(
        AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Scalar));
  }

 private:
  /* DimensionType has 6 values (Scalar..None) using indexes 0-5, and the list
   * bit lives just above them. */
  constexpr static size_t k_listBit = 6;
  constexpr static uint8_t k_listFlag = 1 << k_listBit;
  constexpr static uint8_t k_allTypes =
      AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Scalar) |
      AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Matrix) |
      AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Unit) |
      AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Boolean) |
      AllowedDimensionsInternal::TypeFlag(Internal::DimensionType::Point);

  constexpr explicit AllowedDimensions(uint8_t bits) : m_bits(bits) {}

  uint8_t m_bits;
};

static_assert(sizeof(AllowedDimensions) == 1);

}  // namespace Poincare
