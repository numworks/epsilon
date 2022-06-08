#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <stdint.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

class Expression;
class SymbolAbstract;

class Context {
public:
  enum class SymbolAbstractType : uint8_t {
    None,
    Function,
    Sequence,
    Symbol,
    List
  };
  virtual SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) = 0;
  virtual const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone, float unknownSymbolValue = NAN) = 0;
  virtual bool setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) = 0;
  virtual void tidyDownstreamPoolFrom(char * treePoolCursor = nullptr) {}
};

}

#endif
