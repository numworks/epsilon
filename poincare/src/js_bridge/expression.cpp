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

// TODO: This will be deleted once ReductionContext is binded to js
SystemExpression cloneAndReduce(const UserExpression* expression) {
  EmptyContext context;
  return expression->cloneAndReduce(
      ReductionContext::DefaultReductionContextForAnalysis(&context));
}

// TODO: This will be deleted once ReductionContext is binded to js
UserExpression cloneAndBeautify(const ProjectedExpression* expression) {
  EmptyContext context;
  return expression->cloneAndBeautify(
      ReductionContext::DefaultReductionContextForAnalysis(&context));
}

SystemFunction getSystemFunctionFromString(const SystemExpression* expression,
                                           std::string var) {
  return expression->getSystemFunction(var.c_str(), true);
}

// TODO: This will be deleted once ApproximationContext is binded to js
SystemExpression approximateToTreeDouble(const SystemExpression* expression) {
  EmptyContext context;
  const ApproximationContext approxContext(&context);
  return expression->approximateToTree<double>(approxContext);
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);
  class_<JuniorExpression, base<PoolHandle>>("PCR_Expression")
      .constructor<>()
      .class_function("ParseLatex", &ParseLatexFromString)
      .function("toLatex", &toLatexString, allow_raw_pointers())
      .function("cloneAndReduce", &cloneAndReduce, allow_raw_pointers())
      .function("cloneAndBeautify", &cloneAndBeautify, allow_raw_pointers())
      .function("getSystemFunction", &getSystemFunctionFromString,
                allow_raw_pointers())
      .function("approximateToTree", &approximateToTreeDouble,
                allow_raw_pointers())
      .function("approximateToScalarWithValue",
                &JuniorExpression::approximateToScalarWithValue<double>);
}

}  // namespace Poincare::JSBridge
