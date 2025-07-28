#ifndef OMG_ENUMS_H
#define OMG_ENUMS_H

#include <stdint.h>

namespace OMG {

enum class Base : uint8_t { Binary = 2, Decimal = 10, Hexadecimal = 16 };

enum class Axis : uint8_t {
  Horizontal = 0,
  Vertical,
};

constexpr static OMG::Axis OtherAxis(OMG::Axis axis) {
  return static_cast<OMG::Axis>(1 - static_cast<uint8_t>(axis));
}

}  // namespace OMG

#endif
