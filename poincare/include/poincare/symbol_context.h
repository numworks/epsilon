#pragma once

#include <stdint.h>

#include <string_view>

/* TODO: refactor to have an Internal SymbolContext and a public API
 * SymbolContext */

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

class ContextWithParent;

class SymbolContext {
  friend class ContextWithParent;

 public:
  enum class SymbolType : uint8_t { None, Function, Sequence, Variable, List };

  SymbolType symbolTypeForIdentifier(std::string_view identifier) const {
    return symbolDescriptionForIdentifier(identifier).type;
  }

  /* Returns the tree stored in the context associated with the symbol. The
   * returned Tree* may live in the Pool or in the Storage. The symbol type
   * in the context is checked against the Tree* type. If they don't match
   * nullptr is returned.
   * Ex: if `f` is stored as a Function in the context but `symbol` is the
   * UserVariable `f`, nullptr is returned.
   * TODO: return an Expression type*/
  const Internal::Tree* expressionForSymbol(const Internal::Tree* symbol) const;

#if POINCARE_UNIT
  /* If true, "g" will never be parsed as a unit, and a unit will never
   * be layoutted without "_" (for solver for example)
   * It doesn't apply to some reserved units names (e.g. angle units).
   * This needs to be in the symbol context because it's used both in parsing
   * and layouting, and it's related to how symbols are interpreted. */
  virtual bool useStrictUnitLayout() const { return false; }
#endif

 protected:
  struct SymbolDescription {
    SymbolType type = SymbolType::None;
    // nullptr if type is None or no expression is associated to the symbol
    const Internal::Tree* expression = nullptr;
  };

  virtual SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const = 0;
};

class EmptySymbolContext : public SymbolContext {
 protected:
  SymbolDescription symbolDescriptionForIdentifier(
      std::string_view identifier) const override {
    return {};
  }
};

static const EmptySymbolContext k_emptySymbolContext;

}  // namespace Poincare
