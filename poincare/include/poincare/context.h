#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <stdint.h>

#include <cmath>

// TODO: refactor to have an Internal Context and a public API Context

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

// TODO: Rename to a more specific name
class Context {
 public:
  enum class UserNamedType : uint8_t { None, Function, Sequence, Symbol, List };
  virtual UserNamedType expressionTypeForIdentifier(const char* identifier,
                                                    int length) const = 0;

  // TODO: return an Expression type
  /* The returned Tree* may live in the Pool or in the Storage. */
  virtual const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const = 0;

  virtual double approximateSequenceAtRank(const char* identifier,
                                           int rank) const {
    return NAN;
  }

  static inline void Init(Context* globalContext) {
    GlobalContext = globalContext;
  }

  // TODO: remove, only a static SequenceContext may still be needed
  static inline Context* GlobalContext = nullptr;
};

class EmptyContext : public Context {
 public:
  // Context
  UserNamedType expressionTypeForIdentifier(const char* identifier,
                                            int length) const override {
    return UserNamedType::None;
  }

  const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) const override {
    return nullptr;
  }
};

static const EmptyContext k_emptyContext;

}  // namespace Poincare

#endif
