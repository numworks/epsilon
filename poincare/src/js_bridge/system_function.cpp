#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

#include <string>

#include "expression_types.h"
#include "utils.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// === 1. Builder ===

/* Bind JavaScript type
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
 * This type is defined in poincare-partial.js
 * */
EMSCRIPTEN_DECLARE_VAL_TYPE(SystemFunctionTree);

TypedSystemFunction BuildFromJsTree(const SystemFunctionTree& jsTree) {
  Tree* tree = Utils::JsArrayToTree(jsTree);
  JuniorExpression result = TypedSystemFunction::Builder(tree);
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

/* The following function is used to build Uint8Array from a SystemFunction.
 * It relies on the fact that SystemFunctionTree is globally defined in
 * poincare-partial.js */

SystemFunctionTree ExpressionToJsTree(const TypedSystemFunction& expression) {
  /* Equivalent to the js code "new SystemFunctionTree(typedArray)"
   * This array will be instantiated on javascript heap, allowing it to be
   * properly handled by the js garbage collector */
  return SystemFunctionTree(val::global("SystemFunctionTree")
                                .new_(Utils::TreeToJsArray(expression.tree())));
}

// === 2. Methods ===

double typedApproximateToScalarWithValue(const TypedSystemFunction& expr,
                                         double value) {
  return expr.approximateToScalarWithValue(value);
}

double typedApproximateIntegralToScalar(
    const TypedSystemFunction& expr, const TypedSystemExpression& upperBound,
    const TypedSystemExpression& lowerBound) {
  return expr.approximateIntegralToScalar<double>(upperBound, lowerBound);
}

TypedSystemFunction typedClone(const TypedSystemFunction& expr) {
  JuniorExpression result = expr.clone();
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

TypedSystemFunction typedCloneChildAtIndex(const TypedSystemFunction& expr,
                                           int i) {
  JuniorExpression result = expr.cloneChildAtIndex(i);
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

EMSCRIPTEN_BINDINGS(system_function) {
  register_type<SystemFunctionTree>("SystemFunctionTree");
  class_<TypedSystemFunction, base<JuniorExpression>>("PCR_SystemFunction")
      .constructor<>()
      .class_function("BuildFromTree", &BuildFromJsTree)
      .function("getTree", &ExpressionToJsTree)
      .function("clone", &typedClone)
      .function("cloneChildAtIndex", &typedCloneChildAtIndex)
      .function("approximateToScalarWithValue",
                &typedApproximateToScalarWithValue)
      .function("approximateIntegralToScalar",
                &typedApproximateIntegralToScalar);
}

}  // namespace Poincare::JSBridge
