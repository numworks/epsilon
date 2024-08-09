#ifndef POINCARE_JS_BRIDGE_EXPRESSION_TYPES_H
#define POINCARE_JS_BRIDGE_EXPRESSION_TYPES_H

#include <poincare/old/junior_expression.h>

namespace Poincare::JSBridge {

// These are used to enforce a strong typing system in PoincareJS

class TypedUserExpression : public UserExpression {
 public:
  TypedUserExpression() : UserExpression() {}
};

class TypedSystemExpression : public SystemExpression {
 public:
  TypedSystemExpression() : SystemExpression() {}
};

class TypedSystemFunction : public SystemFunction {
 public:
  TypedSystemFunction() : SystemFunction() {}
};

}  // namespace Poincare::JSBridge
#endif
