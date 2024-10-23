#ifndef POINCARE_JS_BRIDGE_EXPRESSION_TYPES_H
#define POINCARE_JS_BRIDGE_EXPRESSION_TYPES_H

#include <emscripten/bind.h>
#include <poincare/old/junior_expression.h>

namespace Poincare::JSBridge {

// The typed expressions are used to enforce a strong typing system

enum class ExpressionType { UserExpression, SystemExpression, SystemFunction };

template <ExpressionType T>
class TypedExpression : public JuniorExpression {
 public:
  /* Bind JavaScript val type. It represents an Uint8Array which is used to
   * store typed trees from Poincare in the JS heap.
   * This relies on the fact that  the JsTree types are globally defined in
   * poincare-partial.js
   * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
   * */
  EMSCRIPTEN_DECLARE_VAL_TYPE(JsTree)

  TypedExpression() : JuniorExpression() {}

  static inline TypedExpression Cast(JuniorExpression expr) {
    /* WARNING: This is safe as long as TypedExpression doesn't have more
     * attributes than JuniorExpression. */
    static_assert(sizeof(TypedExpression) == sizeof(JuniorExpression));
    return *reinterpret_cast<TypedExpression*>(&expr);
  }

  // Build a JuniorExpression from a JS Uint8Array
  static TypedExpression BuildFromJsTree(const JsTree& jsTree);
  // Build a JS Uint8Array from a JuniorExpression.
  JsTree getJsTree() const;

  TypedExpression typedClone() const;
  TypedExpression typedCloneChildAtIndex(int i) const;
};

using TypedUserExpression = TypedExpression<ExpressionType::UserExpression>;
using TypedSystemExpression = TypedExpression<ExpressionType::SystemExpression>;
using TypedSystemFunction = TypedExpression<ExpressionType::SystemFunction>;

}  // namespace Poincare::JSBridge
#endif
