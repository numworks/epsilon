#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/rational.h>

#include <string>

#include "expression_types.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// === 1. Builders ===

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

EMSCRIPTEN_BINDINGS(system_expression) {
  register_type<TypedSystemExpression::JsTree>("SystemExpressionTree");
  class_<TypedSystemExpression, base<JuniorExpression>>("PCR_SystemExpression")
      .constructor<>()
      .class_function("BuildFromTree", &TypedSystemExpression::BuildFromJsTree)
      .function("getTree", &TypedSystemExpression::getJsTree)
      .function("clone", &TypedSystemExpression::typedClone)
      .function("cloneChildAtIndex",
                &TypedSystemExpression::typedCloneChildAtIndex)
      .class_function("BuildInt", &BuildSystemInt)
      .class_function("BuildFloat", &BuildSystemFloat)
      .class_function("BuildRational", &BuildSystemRational)
      .function("cloneAndBeautify", &typedCloneAndBeautify)
      .function("getSystemFunction", &typedGetSystemFunction)
      .function("getReducedDerivative", &typedGetReducedDerivative)
      .function("approximateToTree", &typedApproximateToTree)
      .function("approximateToScalar", &typedApproximateToScalar);
}

}  // namespace Poincare::JSBridge
