#include <poincare/helpers/symbol.h>
#include <poincare/old/serialization_helper.h>
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

bool SymbolHelper::IsSymbol(NewExpression e, CodePoint c) {
  if (!e.isUserSymbol()) {
    return false;
  }
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointLength =
      SerializationHelper::CodePoint(buffer, bufferSize - 1, c);
  assert(codePointLength < bufferSize);
  return strcmp(GetName(e), buffer) == 0;
}

const char* SymbolHelper::GetName(NewExpression e) {
  return Internal::Symbol::GetName(e.tree());
}

}  // namespace Poincare
