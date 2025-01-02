#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <assert.h>
#include <stdint.h>

#include <cmath>

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

class PoolObject;

class Context {
 public:
  enum class UserNamedType : uint8_t { None, Function, Sequence, Symbol, List };
  virtual UserNamedType expressionTypeForIdentifier(const char* identifier,
                                                    int length) = 0;

  /* The returned Tree* may live in the Pool or in the Storage. */
  virtual const Internal::Tree* expressionForUserNamed(
      const Internal::Tree* symbol) = 0;

  virtual bool setExpressionForUserNamed(const Internal::Tree* expression,
                                         const Internal::Tree* symbol) = 0;
  virtual bool canRemoveUnderscoreToUnits() const { return true; }

  virtual double approximateSequenceAtRank(const char* identifier,
                                           int rank) const {
    return NAN;
  }

  static Context* GlobalContext;
};

}  // namespace Poincare

#endif
