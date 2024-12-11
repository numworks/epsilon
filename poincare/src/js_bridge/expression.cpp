#include <emscripten/bind.h>
#include <poincare/helpers/symbol.h>
#include <poincare/old/junior_expression.h>

using namespace emscripten;

namespace Poincare::JSBridge {

std::string symbolName(const JuniorExpression& expr) {
  if (!expr.isUserSymbol() && !expr.isUserFunction()) {
    // Only works on symbols expressions
    return std::string();
  }
  const char* name = SymbolHelper::GetName(expr);
  return std::string(name, strlen(name));
}

EMSCRIPTEN_BINDINGS(junior_expression) {
  class_<PoolHandle>("PCR_PoolHandle")
#if POINCARE_TREE_LOG
      .function("log", &PoolHandle::log)
#endif
      .function("isUninitialized", &PoolHandle::isUninitialized);

  class_<JuniorExpression, base<PoolHandle>>("PCR_Expression")
      .function("isIdenticalTo", &JuniorExpression::isIdenticalToJunior)
      .function("isUndefined", &JuniorExpression::isUndefined)
      .function("isNAry", &JuniorExpression::isNAry)
      .function("isApproximate", &JuniorExpression::isApproximate)
      .function("isPercent", &JuniorExpression::isPercent)
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
