#ifndef POINCARE_MEMORY_TYPE_ENUM_H
#define POINCARE_MEMORY_TYPE_ENUM_H

namespace Poincare::Internal {

enum class TypeEnum : uint16_t {
/* Add all the types to the enum,
 * enabled and disabled types are mixed up:
 * NODE(MinusOne) => MinusOne,
 * NODE(Fraction) in layout.h => FractionLayout,
 */
#define NODE_USE(F, N, S) SCOPED_NODE(F),
#define UNDEF_NODE_USE(F) SCOPED_NODE(F),
#include "types.h"
};

struct AnyType {
  consteval AnyType(TypeEnum id) : m_id(id) {}
  constexpr operator uint16_t() const { return static_cast<uint16_t>(m_id); }

  TypeEnum m_id;
};

/* We would like to keep the "case Type::Add:" syntax but with custom
 * ids. All the elements are of the type AnyType and stored in the
 * namespace Type to provide an equivalent syntax. */
namespace Type {
#define NODE_USE(F, N, S) \
  constexpr AnyType SCOPED_NODE(F){TypeEnum::SCOPED_NODE(F)};
// The disabled nodes cast to their value in TypeEnum + 256.
#define UNDEF_NODE_USE(F)                                 \
  constexpr AnyType SCOPED_NODE(F){static_cast<TypeEnum>( \
      256 + static_cast<uint16_t>(TypeEnum::SCOPED_NODE(F)))};
#include "types.h"
};  // namespace Type

/* The EnabledType represents only types available at runtime
 * aka "enabled" and stored in a uint8_t. */
class EnabledType {
 public:
  constexpr EnabledType() {}
  constexpr EnabledType(AnyType type)
      : m_value(static_cast<Types>(static_cast<uint8_t>(type))) {}
  constexpr EnabledType(uint8_t value) : m_value(static_cast<Types>(value)) {}
  constexpr operator uint8_t() const { return static_cast<uint8_t>(m_value); }

 private:
  enum class Types : uint8_t {
/* Add all the types to the enum
 * NODE(MinusOne) => MinusOne,
 * NODE(Fraction) in layout.h => FractionLayout,
 */
#define NODE_USE(F, N, S) SCOPED_NODE(F),
#include "types.h"
  };
  Types m_value;
};

// TODO restore LayoutType behavior
namespace LayoutType {
#define ONLY_LAYOUTS 1
#define NODE_USE(F, N, S) constexpr auto F = Type::F##Layout;
#define UNDEF_NODE_USE(F) constexpr auto F = Type::F##Layout;
#include "types.h"
}  // namespace LayoutType
using LayoutAnyType = EnabledType;

#if 0
enum class LayoutType : uint8_t {
/* Members of LayoutType have the same values as their Type counterpart
 * NODE(Fraction) => Fraction = Type::FractionLayout,
 */
#define ONLY_LAYOUTS 1
#define NODE_USE(F, N, S) F = static_cast<uint8_t>(TypeEnum::F##Layout),
#define UNDEF_NODE_USE(F) F = static_cast<uint8_t>(TypeEnum::F##Layout),
#include "types.h"
};
#endif

}  // namespace Poincare::Internal
#endif
