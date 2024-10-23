#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/memory/tree.h>

#include <string>

#include "expression_types.h"
#include "utils.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// === 1. Builders ===

// === 1.1. Build from Javascript array ===

/* Bind JavaScript type
 * https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#custom-val-definitions
 * This type is defined in poincare-partial.js
 * */
EMSCRIPTEN_DECLARE_VAL_TYPE(SystemExpressionTree);

TypedSystemExpression BuildFromJsTree(const SystemExpressionTree& jsTree) {
  Tree* tree = Utils::JsArrayToTree(jsTree);
  JuniorExpression result = TypedSystemExpression::Builder(tree);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

/* The following function is used to build Uint8Array from a SystemExpression.
 * It relies on the fact that SystemExpressionTree is globally defined in
 * poincare-partial.js */

SystemExpressionTree ExpressionToJsTree(
    const TypedSystemExpression& expression) {
  /* Equivalent to the js code "new SystemExpressionTree(typedArray)"
   * This array will be instantiated on javascript heap, allowing it to be
   * properly handled by the js garbage collector */
  return SystemExpressionTree(
      val::global("SystemExpressionTree")
          .new_(Utils::TreeToJsArray(expression.tree())));
}

// === 1.2. Build from numbers ===

TypedSystemExpression BuildSystemInt(int32_t value) {
  JuniorExpression result = JuniorExpression::Builder(value);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression BuildSystemFloat(double value) {
  JuniorExpression result = JuniorExpression::Builder<double>(value);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression BuildSystemRational(int32_t numerator,
                                          int32_t denominator) {
  JuniorExpression result = JuniorExpression::Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

// === 2. Methods ===

TypedUserExpression typedCloneAndBeautify(
    const TypedSystemExpression& expr,
    const ReductionContext& reductionContext) {
  JuniorExpression result = expr.cloneAndBeautify(reductionContext);
  return *reinterpret_cast<TypedUserExpression*>(&result);
}

TypedSystemFunction typedGetSystemFunction(
    const TypedSystemExpression& expression, std::string symbolName) {
  JuniorExpression result =
      expression.getSystemFunction(symbolName.c_str(), true);
  return *reinterpret_cast<TypedSystemFunction*>(&result);
}

TypedSystemExpression typedGetReducedDerivative(
    const TypedSystemExpression& expression, std::string symbolName,
    int derivationOrder) {
  JuniorExpression result =
      expression.getReducedDerivative(symbolName.c_str(), derivationOrder);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression typedApproximateToTree(
    const TypedSystemExpression& expression) {
  EmptyContext context;
  // Expression is already projected
  ApproximationContext approximationContext(
      &context, Preferences::ComplexFormat::Cartesian,
      Preferences::AngleUnit::Radian);
  JuniorExpression result =
      expression.approximateToTree<double>(approximationContext);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

double typedApproximateToScalar(const TypedSystemExpression& expr) {
  return expr.approximateToScalarJunior<double>();
}

TypedSystemExpression typedClone(const TypedSystemExpression& expr) {
  JuniorExpression result = expr.clone();
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

TypedSystemExpression typedCloneChildAtIndex(const TypedSystemExpression& expr,
                                             int i) {
  JuniorExpression result = expr.cloneChildAtIndex(i);
  return *reinterpret_cast<TypedSystemExpression*>(&result);
}

EMSCRIPTEN_BINDINGS(system_expression) {
  register_type<SystemExpressionTree>("SystemExpressionTree");
  class_<TypedSystemExpression, base<JuniorExpression>>("PCR_SystemExpression")
      .constructor<>()
      .class_function("BuildFromTree", &BuildFromJsTree)
      .class_function("BuildInt", &BuildSystemInt)
      .class_function("BuildFloat", &BuildSystemFloat)
      .class_function("BuildRational", &BuildSystemRational)
      .function("getTree", &ExpressionToJsTree)
      .function("clone", &typedClone)
      .function("cloneChildAtIndex", &typedCloneChildAtIndex)
      .function("cloneAndBeautify", &typedCloneAndBeautify)
      .function("getSystemFunction", &typedGetSystemFunction)
      .function("getReducedDerivative", &typedGetReducedDerivative)
      .function("approximateToTree", &typedApproximateToTree)
      .function("approximateToScalar", &typedApproximateToScalar);
}

}  // namespace Poincare::JSBridge
