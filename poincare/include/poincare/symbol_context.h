#pragma once

#include <stdint.h>

#include <string_view>

/* TODO: refactor to have an Internal SymbolContext and a public API
 * SymbolContext */

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

class SymbolContext {
 public:
  enum class UserNamedType : uint8_t { None, Function, Sequence, Symbol, List };
  virtual UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const = 0;

  // TODO: return an Expression type
  /* The returned Tree* may live in the Pool or in the Storage. */
  virtual const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const = 0;

  /* If true, "g" will never be parsed as a unit, and a unit will never
   * be layoutted without "_" (for solver for example)
   * It doesn't apply to some reserved units names (e.g. angle units).
   * This needs to be in the symbol context because it's used both in parsing
   * and layouting, and it's related to how symbols are interpreted. */
  virtual bool isUnitUnderscoreMandatory() const { return false; }
};

class EmptySymbolContext : public SymbolContext {
 public:
  // SymbolContext
  UserNamedType expressionTypeForIdentifier(
      std::string_view identifer) const override {
    return UserNamedType::None;
  }

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override {
    return nullptr;
  }
};

static const EmptySymbolContext k_emptySymbolContext;

}  // namespace Poincare
