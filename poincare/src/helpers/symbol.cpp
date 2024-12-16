#include <omg/utf8_helper.h>
#include <poincare/helpers/symbol.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/memory/tree_stack.h>

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

UserExpression SymbolHelper::BuildSymbol(const char* name, int length) {
  if (length < 0) {
    length = strlen(name);
  }
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
    length = strlen(name);
  }
  return Expression::Builder(
      SharedTreeStack->pushUserSymbol(name, static_cast<size_t>(length + 1)));
}

UserExpression SymbolHelper::BuildSymbol(CodePoint name) {
  constexpr size_t bufferSize = CodePoint::MaxCodePointCharLength + 1;
  char buffer[bufferSize];
  size_t codePointLength =
      UTF8Helper::WriteCodePoint(buffer, bufferSize - 1, name);
  assert(codePointLength < bufferSize);
  return BuildSymbol(buffer, codePointLength);
}

UserExpression SymbolHelper::BuildFunction(const char* name, Expression child) {
  if (BuiltinsAliases::k_thetaAliases.contains(name)) {
    name = BuiltinsAliases::k_thetaAliases.mainAlias();
  }
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserFunction(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  return NewExpression::Builder(e);
}

UserExpression SymbolHelper::BuildSequence(const char* name, Expression child) {
  // If needed, handle theta like functions and symbols
  assert(!BuiltinsAliases::k_thetaAliases.contains(name));
  Internal::Tree* e =
      Internal::SharedTreeStack->pushUserSequence(name, strlen(name) + 1);
  assert(!child.isUninitialized());
  child.tree()->cloneTree();
  return NewExpression::Builder(e);
}

}  // namespace Poincare
