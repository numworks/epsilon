#include <emscripten/bind.h>
#include <poincare/old/junior_expression.h>
#include <poincare/old/symbol_abstract.h>

using namespace emscripten;

namespace Poincare::JSBridge {

std::string symbolName(const JuniorExpression& expr) {
  if (!expr.isUserSymbol() && !expr.isUserFunction()) {
    // Only works on symbols expressions
    return std::string();
  }
  const char* name = static_cast<const SymbolAbstract&>(expr).name();
  return std::string(name, strlen(name));
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
      .function("isUninitialized", &PoolHandle::isUninitialized);

  class_<OExpression, base<PoolHandle>>("PCR_OExpression")
      .function("isIdenticalTo", &OExpression::isIdenticalTo);

  class_<JuniorExpression, base<OExpression>>("PCR_Expression")
      .function("isUndefined", &JuniorExpression::isUndefined)
      .function("isNAry", &JuniorExpression::isNAry)
      .function("isApproximate", &JuniorExpression::isApproximate)
      .function("isPercent", &JuniorExpression::isPercent)
      .function("isIntegral", &JuniorExpression::isIntegral)
      .function("isDiff", &JuniorExpression::isDiff)
      .function("isBoolean", &JuniorExpression::isBoolean)
      .function("isUserSymbol", &JuniorExpression::isUserSymbol)
      .function("isUserFunction", &JuniorExpression::isUserFunction)
      .function("isFactor", &JuniorExpression::isFactor)
      .function("isNonReal", &JuniorExpression::isNonReal)
      .function("isOpposite", &JuniorExpression::isOpposite)
      .function("isDiv", &JuniorExpression::isDiv)
      .function("isBasedInteger", &JuniorExpression::isBasedInteger)
      .function("isComparison", &JuniorExpression::isComparison)
      .function("isEquality", &JuniorExpression::isEquality)
      .function("isRational", &JuniorExpression::isRational)
      .function("isConstantNumber", &JuniorExpression::isConstantNumber)
      .function("symbolName", &symbolName);
}

}  // namespace Poincare::JSBridge
