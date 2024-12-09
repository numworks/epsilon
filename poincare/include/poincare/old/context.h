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
  enum class SymbolAbstractType : uint8_t {
    None,
    Function,
    Sequence,
    Symbol,
    List
  };
  virtual SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                         int length) = 0;

  /* The returned Tree* may live in the Pool or in the Storage. */
  virtual const Internal::Tree* expressionForSymbolAbstract(
      const Internal::Tree* symbol) = 0;

  virtual bool setExpressionForSymbolAbstract(const Internal::Tree* expression,
                                              const Internal::Tree* symbol) = 0;
  virtual bool canRemoveUnderscoreToUnits() const { return true; }

  virtual double approximateSequenceAtRank(const char* identifier,
                                           int rank) const {
    return NAN;
  }

#if POINCARE_CONTEXT_TIDY_POOL
  // TODO: this method should be removed completely
  virtual void tidyDownstreamPoolFrom(PoolObject* treePoolCursor = nullptr) {}
#endif

  static Context* GlobalContext;
};

}  // namespace Poincare

#endif
