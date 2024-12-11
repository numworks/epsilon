#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/rational.h>

#include <string>

#include "typed_expression.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

// === 1. Builders ===

TypedSystemExpression BuildSystemInt(int32_t value) {
  Expression result = Expression::Builder(value);
  return TypedSystemExpression::Cast(result);
}

TypedSystemExpression BuildSystemFloat(double value) {
  Expression result = Expression::Builder<double>(value);
  return TypedSystemExpression::Cast(result);
}

TypedSystemExpression BuildSystemRational(int32_t numerator,
                                          int32_t denominator) {
  Expression result = Expression::Builder(
      Rational::Push(IntegerHandler(numerator), IntegerHandler(denominator)));
  return TypedSystemExpression::Cast(result);
}

// === 2. Methods ===

TypedUserExpression typedCloneAndBeautify(
    const TypedSystemExpression& expr,
    const ReductionContext& reductionContext) {
  Expression result = expr.cloneAndBeautify(reductionContext);
  return TypedUserExpression::Cast(result);
}

TypedSystemFunction typedGetSystemFunction(
    const TypedSystemExpression& expression, std::string symbolName) {
  Expression result = expression.getSystemFunction(symbolName.c_str(), true);
  return TypedSystemFunction::Cast(result);
}

TypedSystemExpression typedGetReducedDerivative(
    const TypedSystemExpression& expression, std::string symbolName,
    int derivationOrder) {
  Expression result =
      expression.getReducedDerivative(symbolName.c_str(), derivationOrder);
  return TypedSystemExpression::Cast(result);
}

TypedSystemExpression typedApproximateToTree(
    const TypedSystemExpression& expression) {
  EmptyContext context;
  // Expression is already projected
  ApproximationContext approximationContext(
      &context, Preferences::ComplexFormat::Cartesian,
      Preferences::AngleUnit::Radian);
  Expression result =
      expression.approximateToTree<double>(approximationContext);
  return TypedSystemExpression::Cast(result);
}

double typedApproximateToScalar(const TypedSystemExpression& expr) {
  return expr.approximateSystemToScalar<double>();
}

EMSCRIPTEN_BINDINGS(system_expression) {
  register_type<TypedSystemExpression::JsTree>("SystemExpressionTree");
  class_<TypedSystemExpression, base<Expression>>("PCR_SystemExpression")
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
      .function("approximateToScalar", &typedApproximateToScalar)
      .function("isPlusOrMinusInfinity", &Expression::isPlusOrMinusInfinity);
}

}  // namespace Poincare::JSBridge
