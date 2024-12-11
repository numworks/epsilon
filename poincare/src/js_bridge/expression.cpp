#include <emscripten/bind.h>
#include <poincare/helpers/symbol.h>
#include <poincare/old/junior_expression.h>

using namespace emscripten;

namespace Poincare::JSBridge {

std::string symbolName(const Expression& expr) {
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
      .function("isIdenticalTo", &Expression::isIdenticalTo)
      .function("isUndefined", &Expression::isUndefinedOrNonReal)
      .function("isUserSymbol", &Expression::isUserSymbol)
      .function("isUserFunction", &Expression::isUserFunction)
      .function("isEquality", &Expression::isEquality)
      .function("symbolName", &symbolName);
}

}  // namespace Poincare::JSBridge
