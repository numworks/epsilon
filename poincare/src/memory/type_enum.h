#pragma once

namespace Poincare::Internal {

enum class TypeEnum : uint8_t {
/* Add all the types to the enum,
 * enabled and disabled types are mixed up:
 * NODE(MinusOne) => MinusOne,
 * NODE(Fraction) in layout.h => FractionLayout,
 */
#define NODE_USE(F, N, S) SCOPED_NODE(F),
#include "types.inc"
};

/* The TypeEnum above keeps a stable, product-independent ordinal for every type
 * (enabled and disabled). Stored type bytes and switch cases use a compacted id
 * instead: disabled ordinals are skipped so enabled types are contiguous. This
 * keeps dispatch jump tables dense. RANGE membership is still computed from the
 * full ordinals (so disabled boundaries remain valid) and remapped to compacted
 * bounds, see type_block.h. */
namespace TypeCompaction {
constexpr bool kEnabled[] = {
#define NODE_USE(F, N, S) true,
#define DISABLED_NODE_USE(F, N, S) false,
#include "types.inc"
};
constexpr uint8_t kNumberOfTypes = sizeof(kEnabled) / sizeof(kEnabled[0]);

/* Prefix sum built once: values[i] = number of enabled types with an ordinal
 * strictly lower than i (so values[ordinal] is the compacted id of an enabled
 * type, and the table doubles as range-bound lookups). */
struct PrefixTable {
  uint8_t values[kNumberOfTypes + 1];
  consteval PrefixTable() : values{} {
    uint8_t compact = 0;
    for (uint8_t i = 0; i < kNumberOfTypes; i++) {
      values[i] = compact;
      compact += kEnabled[i];
    }
    values[kNumberOfTypes] = compact;
  }
};
constexpr PrefixTable kPrefix{};

// Compacted id of an enabled type from its full ordinal.
constexpr uint8_t CompactId(uint8_t ordinal) { return kPrefix.values[ordinal]; }

/* The enabled members of the full-ordinal range [first, last] map to the
 * contiguous compacted range [FirstCompact, LastCompact]. */
constexpr bool RangeIsDisabled(uint8_t first, uint8_t last) {
  return kPrefix.values[last + 1] == kPrefix.values[first];
}
constexpr uint8_t FirstCompact(uint8_t first) { return kPrefix.values[first]; }
constexpr uint8_t LastCompact(uint8_t last) {
  return kPrefix.values[last + 1] - 1;
}
}  // namespace TypeCompaction

struct AnyType {
  static consteval AnyType Enabled(TypeEnum e) {
    return AnyType(TypeCompaction::CompactId(static_cast<uint8_t>(e)));
  }
  static consteval AnyType Disabled(TypeEnum e) {
    /* Types disabled have an id > UINT8_MAX for the compiler to
     * ignore them when they serve as a switch case on a uint8_t
     * switch. */
    return AnyType(static_cast<uint16_t>(static_cast<uint8_t>(e) + 256));
  }
  constexpr bool isEnabled() const { return m_id < 256; }
  constexpr operator uint16_t() const { return m_id; }

  constexpr uint16_t id() const { return m_id; }
  uint16_t m_id;

 private:
  consteval AnyType(uint16_t id) : m_id(id) {}
};

/* We would like to keep the "case Type::Add:" syntax but with custom
 * ids. All the elements are of the type AnyType and stored as static
 * members of Type to provide an equivalent syntax. */
class Type {
 public:
#define NODE_USE(F, N, S)                   \
  constexpr static AnyType SCOPED_NODE(F) = \
      AnyType::Enabled(TypeEnum::SCOPED_NODE(F));
#define DISABLED_NODE_USE(F, N, S)          \
  constexpr static AnyType SCOPED_NODE(F) = \
      AnyType::Disabled(TypeEnum::SCOPED_NODE(F));
#include "types.inc"

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
#define NODE_USE(F, N, S) constexpr static auto F = Type::F##Layout;
#include "types.inc"

  constexpr LayoutType(AnyType type)
      : m_value(static_cast<TypeEnum>(static_cast<uint8_t>(type))) {
    assert(type.isEnabled());
  }

  constexpr operator Type() const {
    return Type(static_cast<uint8_t>(m_value));
  }
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_value); }
  constexpr bool operator==(const LayoutType&) const = default;
  constexpr bool operator==(const AnyType& type) const {
    return static_cast<uint8_t>(m_value) == type.id();
  }

 private:
  constexpr LayoutType(Type type)
      : m_value(static_cast<TypeEnum>(static_cast<uint8_t>(type))) {
    // Tree::layoutType will assert type.isLayout
  }
  TypeEnum m_value;
};

}  // namespace Poincare::Internal
