#ifndef POINCARE_MEMORY_TYPE_ENUM_H
#define POINCARE_MEMORY_TYPE_ENUM_H

namespace Poincare::Internal {

enum class TypeEnum : uint8_t {
/* Add all the types to the enum,
 * enabled and disabled types are mixed up:
 * NODE(MinusOne) => MinusOne,
 * NODE(Fraction) in layout.h => FractionLayout,
 */
#define NODE_USE(F, N, S) SCOPED_NODE(F),
#include "types.h"
};

struct AnyType {
  static consteval AnyType Enabled(TypeEnum e) {
    return {static_cast<uint8_t>(e)};
  }
  static consteval AnyType Disabled(TypeEnum e) {
    /* Types disabled have an id > UINT8_MAX for the compiler to
     * ignore them when they serve as a switch case on a uint8_t
     * switch. */
    return {static_cast<uint16_t>(static_cast<uint8_t>(e) + UINT8_MAX)};
  }
  constexpr bool isEnabled() const { return m_id < UINT8_MAX; }
  constexpr operator uint16_t() const { return m_id; }

  uint16_t m_id;
};

/* We would like to keep the "case Type::Add:" syntax but with custom
 * ids. All the elements are of the type AnyType and stored as static
 * members of Type to provide an equivalent syntax. */
class Type {
 public:
#define NODE_USE(F, N, S)                   \
  static constexpr AnyType SCOPED_NODE(F) = \
      AnyType::Enabled(TypeEnum::SCOPED_NODE(F));
#define DISABLED_NODE_USE(F, N, S)          \
  static constexpr AnyType SCOPED_NODE(F) = \
      AnyType::Disabled(TypeEnum::SCOPED_NODE(F));
#include "types.h"

  constexpr Type() {}
  constexpr Type(AnyType type)
      : m_value(static_cast<TypeEnum>(static_cast<uint8_t>(type))) {
    assert(type.isEnabled());
  }
  constexpr Type(uint8_t value) : m_value(static_cast<TypeEnum>(value)) {}
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_value); }

 private:
  TypeEnum m_value;
};

class LayoutType {
  friend class Tree;

 public:
#define ONLY_LAYOUTS 1
/* Members of LayoutType have the same values as their Type counterpart
 * NODE(Fraction) => Fraction = Type::FractionLayout,
 */
#define NODE_USE(F, N, S) static constexpr auto F = Type::F##Layout;
#include "types.h"

  constexpr LayoutType(AnyType type)
      : m_value(static_cast<TypeEnum>(static_cast<uint8_t>(type))) {
    assert(type.isEnabled());
  }

  constexpr operator Type() const {
    return Type(static_cast<uint8_t>(m_value));
  }
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_value); }
  constexpr bool operator==(const LayoutType&) const = default;

 private:
  constexpr LayoutType(Type type)
      : m_value(static_cast<TypeEnum>(static_cast<uint8_t>(type))) {
    // Tree::layoutType will assert type.isLayout
  }
  TypeEnum m_value;
};
using LayoutAnyType = LayoutType;

}  // namespace Poincare::Internal
#endif
