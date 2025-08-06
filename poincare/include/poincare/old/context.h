#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <stdint.h>

#include <cmath>

// TODO: this file should be moved to the "new" Poincare.

// TODO: no need to forward-declare Tree once moved to the new Poincare
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

  // TODO: remove, only a static SequenceContext may still be needed
  static Context* GlobalContext;
};

}  // namespace Poincare

#endif
