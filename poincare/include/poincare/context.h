#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <stdint.h>
#include <assert.h>

namespace Poincare {

class Expression;
class SymbolAbstract;

class Context {
public:
  enum class SymbolAbstractType : uint8_t {
    None,
    Function,
    Symbol
  };
  virtual SymbolAbstractType expressionTypeForIdentifier(const char * identifier, int length) = 0;
  virtual const Expression expressionForSymbolAbstract(const SymbolAbstract & symbol, bool clone) = 0;
  virtual void setExpressionForSymbolAbstract(const Expression & expression, const SymbolAbstract & symbol) = 0;
};

}

#endif
