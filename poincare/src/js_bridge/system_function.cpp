#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/src/expression/projection.h>

#include <string>

#include "typed_expression.h"

using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

double typedApproximateToScalarWithValue(const TypedSystemFunction& expr,
                                         double value) {
  return expr.approximateToScalarWithValue(value);
}

double typedApproximateIntegralToScalar(
    const TypedSystemFunction& expr, const TypedSystemExpression& upperBound,
    const TypedSystemExpression& lowerBound) {
  return expr.approximateIntegralToScalar<double>(upperBound, lowerBound);
}

EMSCRIPTEN_BINDINGS(system_function) {
  register_type<TypedSystemFunction::JsTree>("SystemFunctionTree");
  class_<TypedSystemFunction, base<Expression>>("PCR_SystemFunction")
      .constructor<>()
      .class_function("BuildFromTree", &TypedSystemFunction::BuildFromJsTree)
      .function("getTree", &TypedSystemFunction::getJsTree)
      .function("clone", &TypedSystemFunction::typedClone)
      .function("cloneChildAtIndex",
                &TypedSystemFunction::typedCloneChildAtIndex)
      .function("approximateToScalarWithValue",
                &typedApproximateToScalarWithValue)
      .function("approximateIntegralToScalar",
                &typedApproximateIntegralToScalar);
}

}  // namespace Poincare::JSBridge
