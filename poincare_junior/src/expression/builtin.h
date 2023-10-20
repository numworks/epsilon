#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "aliases.h"

namespace PoincareJ {

class Builtin : public std::pair<BlockType, Aliases> {
 public:
  using pair::pair;
  const BlockType blockType() const { return first; }
  const Aliases* aliases() const { return &second; }
  Tree* pushNode() const { return SharedEditionPool->push(first); }
  static bool IsReservedFunction(BlockType type) {
    return GetReservedFunction(type) != nullptr;
  }
  static Aliases ReservedFunctionName(BlockType type) {
    assert(GetReservedFunction(type));
    return GetReservedFunction(type)->second;
  }
  static Aliases SpecialIdentifierName(BlockType type) {
    assert(GetSpecialIdentifier(type));
    return GetSpecialIdentifier(type)->second;
  }
  static bool HasReservedFunction(UnicodeDecoder* name) {
    return GetReservedFunction(name) != nullptr;
  }
  static bool HasSpecialIdentifier(UnicodeDecoder* name) {
    return GetSpecialIdentifier(name) != nullptr;
  }
  static bool HasCustomIdentifier(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(UnicodeDecoder* name);
  static const Builtin* GetReservedFunction(BlockType type);
  static const Builtin* GetSpecialIdentifier(UnicodeDecoder* name);
  static const Builtin* GetSpecialIdentifier(BlockType type);
  static uint8_t MinNumberOfParameters(BlockType type) {
    return TypeBlock::NumberOfChildren(type);
  }
  static uint8_t MaxNumberOfParameters(BlockType type) {
    return MinNumberOfParameters(type);
  }
  static bool Promote(Tree* parameterList, const Builtin* builtin);
  EDITION_REF_WRAP_1(Promote, const Builtin*);
};

namespace BuiltinsAliases {
// Special identifiers
constexpr static Aliases k_ansAliases = "\01Ans\00ans\00";
constexpr static Aliases k_trueAliases = "\01True\00true\00";
constexpr static Aliases k_falseAliases = "\01False\00false\00";
constexpr static Aliases k_infinityAliases = "\01∞\00inf\00";
// Constants
constexpr static Aliases k_piAliases = "\01π\00pi\00";
// Symbols
constexpr static Aliases k_thetaAliases = "\01θ\00theta\00";
// Units
constexpr static Aliases k_litersAliases = "\01L\00l\00";
// Inverse trigo
constexpr static Aliases k_acosAliases = "\01arccos\00acos\00";
constexpr static Aliases k_asinAliases = "\01arcsin\00asin\00";
constexpr static Aliases k_atanAliases = "\01arctan\00atan\00";
// Other functions
constexpr static Aliases k_squareRootAliases = "\01√\00sqrt\00";
}  // namespace BuiltinsAliases

}  // namespace PoincareJ

#endif
