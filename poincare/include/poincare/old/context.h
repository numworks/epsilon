#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <assert.h>
#include <stdint.h>

#include <cmath>

namespace Poincare::Internal {
class Tree;
}

namespace Poincare {

class ContextWithParent;
class PoolObject;

class Context {
  friend class ContextWithParent;

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
  const Internal::Tree* expressionForSymbolAbstract(
      const Internal::Tree* symbol);

  virtual bool setExpressionForSymbolAbstract(const Internal::Tree* expression,
                                              const Internal::Tree* symbol) = 0;
  virtual void tidyDownstreamPoolFrom(PoolObject* treePoolCursor = nullptr) {}
  virtual bool canRemoveUnderscoreToUnits() const { return true; }

  virtual double approximateSequenceAtRank(const char* identifier,
                                           int rank) const {
    return NAN;
  }

  static Context* GlobalContext;

 protected:
  /* This is used by the ContextWithParent to pass itself to its parent.
   * When getting the expression for a sequence in GlobalContext, you need
   * information on the variable that is stored in the ContextWithParent that
   * called you. */
  virtual const Internal::Tree* protectedExpressionForSymbolAbstract(
      const Internal::Tree* symbol,
      ContextWithParent* lastDescendantContext) = 0;
};

}  // namespace Poincare

#endif
