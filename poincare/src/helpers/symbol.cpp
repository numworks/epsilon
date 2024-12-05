#include <poincare/helpers/symbol.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/symbol.h>

namespace Poincare {

using namespace Internal;

const char* SymbolHelper::AnsMainAlias() {
  return BuiltinsAliases::k_ansAliases.mainAlias();
}

bool SymbolHelper::IsTheta(NewExpression e) {
  return e.isUserSymbol() &&
         BuiltinsAliases::k_thetaAliases.contains(GetName(e));
}

const char* SymbolHelper::GetName(NewExpression e) {
  return Internal::Symbol::GetName(e.tree());
}

}  // namespace Poincare
