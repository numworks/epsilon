#include <emscripten/bind.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

#include <string>
using namespace emscripten;

namespace Poincare::JSBridge {

UserExpression ParseLatexFromString(std::string latex) {
  EmptyContext context;
  return JuniorExpression::ParseLatex(latex.c_str(), &context);
}

std::string toLatexString(const UserExpression* expression) {
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  EmptyContext context;
  expression->toLatex(buffer, k_bufferSize,
                      Preferences::PrintFloatMode::Decimal, 7, &context);
  return std::string(buffer, strlen(buffer));
}

SystemFunction getSystemFunctionFromString(const SystemExpression* expression,
                                           std::string var) {
  return expression->getSystemFunction(var.c_str(), true);
}

double ApproximateToScalarWithValue(const JuniorExpression& expr,
                                    double value) {
  return expr.approximateToScalarWithValue(value);
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<OExpression, base<PoolHandle>>("PCR_OExpression");
  class_<JuniorExpression, base<OExpression>>("PCR_Expression")
      .constructor<>()
      .class_function("ParseLatex", &ParseLatexFromString)
      .class_function("ExactAndApproximateExpressionsAreEqual",
                      &OExpression::ExactAndApproximateExpressionsAreEqual)
      .function("toLatex", &toLatexString, allow_raw_pointers())
      .function("cloneAndReduce", &JuniorExpression::cloneAndReduce)
      .function("cloneAndBeautify", &JuniorExpression::cloneAndBeautify)
      .function("getSystemFunction", &getSystemFunctionFromString,
                allow_raw_pointers())
      .function("approximateToTree",
                &JuniorExpression::approximateToTree<double>,
                allow_raw_pointers())
      .function("approximateToScalarWithValue", &ApproximateToScalarWithValue);
}

}  // namespace Poincare::JSBridge
