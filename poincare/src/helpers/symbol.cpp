#include <omg/utf8_helper.h>
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

bool SymbolHelper::IsSymbol(NewExpression e, CodePoint c) {
  if (!e.isUserSymbol()) {
    return false;
  }
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointLength =
      UTF8Helper::WriteCodePoint(buffer, bufferSize - 1, c);
  assert(codePointLength < bufferSize);
  return strcmp(GetName(e), buffer) == 0;
}

const char* SymbolHelper::GetName(NewExpression e) {
  return Internal::Symbol::GetName(e.tree());
}

size_t SymbolHelper::NameWithoutQuotationMarks(char* buffer, size_t bufferSize,
                                               const char* name,
                                               size_t nameLength) {
  if (NameHasQuotationMarks(name, nameLength)) {
    assert(bufferSize > nameLength - 2);
    size_t result = strlcpy(buffer, name + 1, bufferSize) - 1;
    buffer[nameLength - 2] = 0;  // Remove the last '""
    return result;
  }
  assert(bufferSize > nameLength);
  return strlcpy(buffer, name, bufferSize);
}

}  // namespace Poincare
