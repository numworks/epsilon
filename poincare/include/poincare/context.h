#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <stdint.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

class Expression;
class SymbolAbstract;

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
  virtual SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) = 0;
  const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone);
  virtual bool setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) = 0;
  virtual void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) {}
protected:
  virtual const Expression protectedExpressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone, Context * contextWithMoreInformations) = 0;

};

}

#endif
