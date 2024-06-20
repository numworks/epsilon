#include <emscripten/bind.h>
#include <poincare/old/empty_context.h>
#include <poincare/old/junior_expression.h>

#include <string>
using namespace emscripten;

namespace Poincare {

UserExpression JuniorExpression::ParseLatexFromString(std::string latex) {
  EmptyContext context;
  return JuniorExpression::ParseLatex(latex.c_str(), &context);
}

std::string UserExpression::toLatexString() const {
  constexpr int k_bufferSize = 1024;  // TODO: make this bigger ? or malloc ?
  char buffer[k_bufferSize];
  EmptyContext context;
  toLatex(buffer, k_bufferSize, Preferences::PrintFloatMode::Decimal, 7,
          &context);
  return std::string(buffer, strlen(buffer));
}

SystemExpression UserExpression::cloneAndReduce() const {
  EmptyContext context;
  return cloneAndReduce(
      ReductionContext::DefaultReductionContextForAnalysis(&context));
}

UserExpression ProjectedExpression::cloneAndBeautify() const {
  EmptyContext context;
  return cloneAndBeautify(
      ReductionContext::DefaultReductionContextForAnalysis(&context));
}

SystemFunction SystemExpression::getSystemFunctionFromString(
    std::string var) const {
  return getSystemFunction(var.c_str(), true);
}

SystemExpression SystemExpression::approximateToTreeDouble() const {
  EmptyContext context;
  const ApproximationContext approxContext(&context);
  return approximateToTree<double>(approxContext);
}

// Binding code
EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<JuniorExpression, base<PoolHandle>>("PCR_Expression")
      .constructor<>()
      .class_function("ParseLatex", &JuniorExpression::ParseLatexFromString)
      .function("toLatex", &JuniorExpression::toLatexString)
      .function("cloneAndReduce", select_overload<SystemExpression() const>(
                                      &JuniorExpression::cloneAndReduce))
      .function("cloneAndBeautify", select_overload<UserExpression() const>(
                                        &JuniorExpression::cloneAndBeautify))
      .function("getSystemFunction",
                &JuniorExpression::getSystemFunctionFromString)
      .function("approximateToTree", &JuniorExpression::approximateToTreeDouble)
      .function("approximateToScalarWithValue",
                &JuniorExpression::approximateToScalarWithValue<double>);
}

}  // namespace Poincare
