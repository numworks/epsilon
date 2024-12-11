#ifndef POINCARE_JS_BRIDGE_TYPED_EXPRESSION_H
#define POINCARE_JS_BRIDGE_TYPED_EXPRESSION_H

#include <emscripten/bind.h>
#include <poincare/expression.h>

namespace Poincare::JSBridge {

// The typed expressions are used to enforce a strong typing system

enum class ExpressionType { UserExpression, SystemExpression, SystemFunction };

template <ExpressionType T>
class TypedExpression : public Expression {
 public:
  /* Bind JavaScript val type. It represents an Uint8Array which is used to
   * store typed trees from Poincare in the JS heap.
   * This relies on the fact that the JsTree types are globally defined in
   * poincare-partial.js
   * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
   * */
  EMSCRIPTEN_DECLARE_VAL_TYPE(JsTree)

  TypedExpression() : Expression() {}

  static inline TypedExpression Cast(Expression expr) {
    /* WARNING: The down-casting of Expression into TypedExpression
     * is possible only because TypedExpressions don't have more member
     * variables than Expression. */
    static_assert(sizeof(TypedExpression) == sizeof(Expression));
    return *reinterpret_cast<TypedExpression*>(&expr);
  }

  // Build a Expression from a JS Uint8Array
  static TypedExpression BuildFromJsTree(const JsTree& jsTree);
  // Build a JS Uint8Array from a Expression.
  JsTree getJsTree() const;

  TypedExpression typedClone() const;
  TypedExpression typedCloneChildAtIndex(int i) const;
};

using TypedUserExpression = TypedExpression<ExpressionType::UserExpression>;
using TypedSystemExpression = TypedExpression<ExpressionType::SystemExpression>;
using TypedSystemFunction = TypedExpression<ExpressionType::SystemFunction>;

}  // namespace Poincare::JSBridge
#endif
