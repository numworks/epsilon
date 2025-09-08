#pragma once

#include <stdint.h>

#include <string_view>

// TODO: refactor to have an Internal Context and a public API Context

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

class Context {
 public:
  enum class UserNamedType : uint8_t { None, Function, Sequence, Symbol, List };
  virtual UserNamedType expressionTypeForIdentifier(
      std::string_view identifier) const = 0;

  // TODO: return an Expression type
  /* The returned Tree* may live in the Pool or in the Storage. */
  virtual const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const = 0;
};

class EmptySymbolContext : public Context {
 public:
  // Context
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
