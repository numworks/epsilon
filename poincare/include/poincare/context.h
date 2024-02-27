#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <assert.h>
#include <stdint.h>

#include <cmath>

namespace Poincare {

class OExpression;
class SymbolAbstract;
class ContextWithParent;
class TreeNode;

class Context {
  friend class ContextWithParent;

 public:
  enum class SymbolAbstractType : uint8_t {
    None,
    Function,
    Sequence,
    Symbol,
    OList
  };
  virtual SymbolAbstractType expressionTypeForIdentifier(const char* identifier,
                                                         int length) = 0;
  const OExpression expressionForSymbolAbstract(const SymbolAbstract& symbol,
                                                bool clone);
  virtual bool setExpressionForSymbolAbstract(const OExpression& expression,
                                              const SymbolAbstract& symbol) = 0;
  virtual void tidyDownstreamPoolFrom(TreeNode* treePoolCursor = nullptr) {}
  virtual bool canRemoveUnderscoreToUnits() const { return true; }

 protected:
  /* This is used by the ContextWithParent to pass itself to its parent.
   * When getting the expression for a sequences in GlobalContext, you need
   * information on the variable that is stored in the ContextWithParent that
   * called you. */
  virtual const OExpression protectedExpressionForSymbolAbstract(
      const SymbolAbstract& symbol, bool clone,
      ContextWithParent* lastDescendantContext) = 0;
};

}  // namespace Poincare

#endif
