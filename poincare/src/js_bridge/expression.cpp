#include <emscripten/bind.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/memory/tree.h>

#include <string>
using namespace emscripten;
using namespace Poincare::Internal;

namespace Poincare::JSBridge {

std::string toLatexString(const UserExpression* expression,
                          int numberOfSignificantDigits) {
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  EmptyContext context;
  expression->toLatex(buffer, k_bufferSize,
                      Preferences::PrintFloatMode::Decimal,
                      numberOfSignificantDigits, &context);
  return std::string(buffer, strlen(buffer));
}

std::string toLatexStringWith7Digits(const UserExpression* expression) {
  return toLatexString(expression, 7);
}

SystemFunction getSystemFunctionFromString(const SystemExpression* expression,
                                           std::string symbolName) {
  return expression->getSystemFunction(symbolName.c_str(), true);
}

SystemExpression getReducedDerivativeFromString(
    const SystemExpression* expression, std::string symbolName,
    int derivationOrder) {
  return expression->getReducedDerivative(symbolName.c_str(), derivationOrder);
}

double ApproximateToScalarWithValue(const JuniorExpression& expr,
                                    double value) {
  return expr.approximateToScalarWithValue(value);
}

bool ExactAndApproximateExpressionsAreStrictlyEqualWrapper(
    const JuniorExpression& exact, const JuniorExpression& approximate,
    Preferences::ComplexFormat complexFormat,
    Preferences::AngleUnit angleUnit) {
  ProjectionContext ctx{.m_complexFormat = complexFormat,
                        .m_angleUnit = angleUnit};
  return ExactAndApproximateExpressionsAreStrictlyEqual(exact, approximate,
                                                        &ctx);
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<OExpression, base<PoolHandle>>("PCR_OExpression");
  class_<JuniorExpression, base<OExpression>>("PCR_Expression")
      .constructor<>()
      .class_function("ExactAndApproximateExpressionsAreStrictlyEqual",
                      &ExactAndApproximateExpressionsAreStrictlyEqualWrapper)
      .function("tree", &JuniorExpression::tree, allow_raw_pointers())
      .function("toLatex", &toLatexString, allow_raw_pointers())
      .function("toLatex", &toLatexStringWith7Digits, allow_raw_pointers())
      .function("cloneAndReduce", &JuniorExpression::cloneAndReduce)
      .function("cloneAndBeautify", &JuniorExpression::cloneAndBeautify)
      .function("getSystemFunction", &getSystemFunctionFromString,
                allow_raw_pointers())
      .function("getReducedDerivative", getReducedDerivativeFromString,
                allow_raw_pointers())
      .function("approximateToTree",
                &JuniorExpression::approximateToTree<double>,
                allow_raw_pointers())
      .function("approximateToScalarWithValue", &ApproximateToScalarWithValue)
      .function("approximateIntegralToScalar",
                &JuniorExpression::approximateIntegralToScalar<double>);
}

}  // namespace Poincare::JSBridge
